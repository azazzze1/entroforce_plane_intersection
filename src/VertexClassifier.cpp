// VertexClassifier.cpp
#include "intersection/VertexClassifier.hpp"
#include <unordered_map>
#include <cmath>
#include <algorithm>

bool VertexClassifier::isLeft(double fromX, double fromY, double toX, double toY,
                              double pointX, double pointY) {
    return (toX - fromX) * (pointY - fromY) - (toY - fromY) * (pointX - fromX) > 0.0;
}

std::pair<double, double> VertexClassifier::interpolate(const Point3D& v0, const Point3D& v1, double height) {
    double diff = v1.z - v0.z;
    if (std::abs(diff) < 1e-12) {
        return {(v0.x + v1.x) / 2, (v0.y + v1.y) / 2};
    }
    double t = (height - v0.z) / diff;
    return {v0.x + t * (v1.x - v0.x), v0.y + t * (v1.y - v0.y)};
}

std::vector<Segment2D> VertexClassifier::extractSegments(const MeshGraph& graph, double height) {
    std::vector<Segment2D> segments;
    segments.reserve(graph.faces.size() / 2);

    std::vector<VertexState> states(graph.vertices.size());
    for (size_t i = 0; i < graph.vertices.size(); ++i) {
        double diff = graph.vertices[i].z - height;
        if (std::abs(diff) <= EPS) states[i] = VertexState::ON;
        else if (diff < 0.0) states[i] = VertexState::BELOW;
        else states[i] = VertexState::ABOVE;
    }

    // Первый проход: собираем все внутренние отрезки с правильной ориентацией
    std::vector<Segment2D> rawSegments;
    rawSegments.reserve(graph.faces.size());

    for (size_t fi = 0; fi < graph.faces.size(); ++fi) {
        const auto& f = graph.faces[fi];
        VertexState s[3] = {states[f.v0], states[f.v1], states[f.v2]};
        const Point3D* v[3] = {&graph.vertices[f.v0], &graph.vertices[f.v1], &graph.vertices[f.v2]};

        int belowCount = 0, aboveCount = 0, onCount = 0;
        for (int i = 0; i < 3; ++i) {
            if (s[i] == VertexState::BELOW) ++belowCount;
            else if (s[i] == VertexState::ABOVE) ++aboveCount;
            else ++onCount;
        }

        if (belowCount == 3 || aboveCount == 3) continue;   // полностью выше/ниже
        if (onCount == 3) continue;                         // плато на плоскости

        auto addSeg = [&](double x0, double y0, double x1, double y1, bool checkLeft, double refX, double refY) {
            if (checkLeft) {
                if (isLeft(x0, y0, x1, y1, refX, refY))
                    rawSegments.emplace_back(x0, y0, x1, y1, SegmentType::INTERIOR, static_cast<int>(fi));
                else
                    rawSegments.emplace_back(x1, y1, x0, y0, SegmentType::INTERIOR, static_cast<int>(fi));
            } else {
                // ref должна быть справа (для ABOVE)
                if (!isLeft(x0, y0, x1, y1, refX, refY))
                    rawSegments.emplace_back(x0, y0, x1, y1, SegmentType::INTERIOR, static_cast<int>(fi));
                else
                    rawSegments.emplace_back(x1, y1, x0, y0, SegmentType::INTERIOR, static_cast<int>(fi));
            }
        };

        if (belowCount == 1 && aboveCount == 2) {
            int belowIdx = (s[0] == VertexState::BELOW) ? 0 : (s[1] == VertexState::BELOW) ? 1 : 2;
            int e0 = (belowIdx + 1) % 3, e1 = (belowIdx + 2) % 3;
            auto [x1, y1] = interpolate(*v[belowIdx], *v[e0], height);
            auto [x2, y2] = interpolate(*v[belowIdx], *v[e1], height);
            addSeg(x1, y1, x2, y2, true, v[belowIdx]->x, v[belowIdx]->y);
        }
        else if (belowCount == 2 && aboveCount == 1) {
            int aboveIdx = (s[0] == VertexState::ABOVE) ? 0 : (s[1] == VertexState::ABOVE) ? 1 : 2;
            int e0 = (aboveIdx + 1) % 3, e1 = (aboveIdx + 2) % 3;
            auto [x1, y1] = interpolate(*v[aboveIdx], *v[e0], height);
            auto [x2, y2] = interpolate(*v[aboveIdx], *v[e1], height);
            // Одна из соседних вершин BELOW, можно использовать её как референс
            double refX = v[e0]->x, refY = v[e0]->y;  // любая из BELOW
            addSeg(x1, y1, x2, y2, true, refX, refY);
        }
        else if (onCount == 1 && belowCount == 1 && aboveCount == 1) {
            int onIdx = -1, belowIdx = -1, aboveIdx = -1;
            for (int i = 0; i < 3; ++i) {
                if (s[i] == VertexState::ON) onIdx = i;
                else if (s[i] == VertexState::BELOW) belowIdx = i;
                else aboveIdx = i;
            }
            auto [ix, iy] = interpolate(*v[belowIdx], *v[aboveIdx], height);
            addSeg(v[onIdx]->x, v[onIdx]->y, ix, iy, true, v[belowIdx]->x, v[belowIdx]->y);
        }
        else if (onCount == 2) {
            int specialIdx = -1;
            VertexState specialState;
            for (int i = 0; i < 3; ++i) {
                if (s[i] != VertexState::ON) { specialIdx = i; specialState = s[i]; break; }
            }
            int on1 = (specialIdx + 1) % 3;
            int on2 = (specialIdx + 2) % 3;
            if (specialState == VertexState::BELOW) {
                addSeg(v[on1]->x, v[on1]->y, v[on2]->x, v[on2]->y, true, v[specialIdx]->x, v[specialIdx]->y);
            } else { // ABOVE
                addSeg(v[on1]->x, v[on1]->y, v[on2]->x, v[on2]->y, false, v[specialIdx]->x, v[specialIdx]->y);
            }
        }
    }

    // Удаление вырожденных ON-ON рёбер, не разделяющих BELOW и ABOVE
    struct EdgeKey {
        int v0, v1;
        bool operator==(const EdgeKey& o) const {
            return (v0 == o.v0 && v1 == o.v1) || (v0 == o.v1 && v1 == o.v0);
        }
    };
    struct EdgeKeyHash {
        size_t operator()(const EdgeKey& e) const {
            int a = std::min(e.v0, e.v1);
            int b = std::max(e.v0, e.v1);
            return std::hash<int>{}(a) ^ (std::hash<int>{}(b) << 1);
        }
    };
    std::unordered_map<EdgeKey, std::vector<int>, EdgeKeyHash> edgeToFaces;
    for (size_t fi = 0; fi < graph.faces.size(); ++fi) {
        const auto& f = graph.faces[fi];
        edgeToFaces[{f.v0, f.v1}].push_back(static_cast<int>(fi));
        edgeToFaces[{f.v1, f.v2}].push_back(static_cast<int>(fi));
        edgeToFaces[{f.v2, f.v0}].push_back(static_cast<int>(fi));
    }

    for (const auto& seg : rawSegments) {
        if (seg.type != SegmentType::INTERIOR) {
            segments.push_back(seg);
            continue;
        }
        const Face& f = graph.faces[seg.faceIndex];
        bool v0_on = (states[f.v0] == VertexState::ON);
        bool v1_on = (states[f.v1] == VertexState::ON);
        bool v2_on = (states[f.v2] == VertexState::ON);
        int onCount = (v0_on ? 1 : 0) + (v1_on ? 1 : 0) + (v2_on ? 1 : 0);
        if (onCount != 2) {
            segments.push_back(seg);  // не ON-ON, оставляем
            continue;
        }

        // Определяем индексы ON-вершин
        int on1 = -1, on2 = -1;
        if (v0_on && v1_on) { on1 = f.v0; on2 = f.v1; }
        else if (v1_on && v2_on) { on1 = f.v1; on2 = f.v2; }
        else if (v2_on && v0_on) { on1 = f.v2; on2 = f.v0; }

        EdgeKey key{on1, on2};
        auto it = edgeToFaces.find(key);
        if (it == edgeToFaces.end()) { segments.push_back(seg); continue; }

        bool allBelow = true, allAbove = true;
        for (int faceIdx : it->second) {
            const Face& nf = graph.faces[faceIdx];
            int third = -1;
            if (nf.v0 != on1 && nf.v0 != on2) third = nf.v0;
            else if (nf.v1 != on1 && nf.v1 != on2) third = nf.v1;
            else third = nf.v2;
            VertexState st = states[third];
            if (st != VertexState::BELOW) allBelow = false;
            if (st != VertexState::ABOVE) allAbove = false;
        }
        if (!allBelow && !allAbove) {
            segments.push_back(seg);  // разделяет зоны
        }
        // иначе плато – отбрасываем
    }

    return segments;
}
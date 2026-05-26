// ContourCreater.cpp
#include "intersection/ContourCreater.hpp"
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <set>
#include <cassert>

using namespace std;

bool ContourCreater::isPointOnSegment(double px, double py, double ax, double ay, double bx, double by) {
    double cross = (px - ax) * (by - ay) - (py - ay) * (bx - ax);
    if (fabs(cross) > 1e-6) return false;
    double dot = (px - ax) * (bx - ax) + (py - ay) * (by - ay);
    if (dot < -1e-6) return false;
    double len2 = (bx - ax) * (bx - ax) + (by - ay) * (by - ay);
    if (dot > len2 + 1e-6) return false;
    return true;
}

double ContourCreater::projectOnEdge(double px, double py, double ax, double ay, double bx, double by) {
    double len2 = (bx - ax) * (bx - ax) + (by - ay) * (by - ay);
    if (len2 < 1e-12) return 0.0;
    double t = ((px - ax) * (bx - ax) + (py - ay) * (by - ay)) / len2;
    return max(0.0, min(1.0, t));
}

void ContourCreater::addBoundarySegments(const ParsedMesh& mesh,
                                         const vector<Segment2D>& interiorSegments,
                                         vector<Segment2D>& allSegments) {
    for (size_t p = 0; p < mesh.polygons.size(); ++p) {
        auto poly = mesh.polygons[p];
        // Ориентация полигона: внешний (p==0) — CCW, дырки — CW
        double area = 0.0;
        for (size_t i = 0; i < poly.vertexIDX.size(); ++i) {
            size_t j = (i + 1) % poly.vertexIDX.size();
            const auto& a = mesh.points[poly.vertexIDX[i]];
            const auto& b = mesh.points[poly.vertexIDX[j]];
            area += a.x * b.y - b.x * a.y;
        }
        bool isOuter = (p == 0);
        if ((isOuter && area < 0) || (!isOuter && area > 0)) {
            reverse(poly.vertexIDX.begin(), poly.vertexIDX.end());
            area = -area; // теперь знак соответствует нужному
        }

        // Для каждого ребра полигона
        for (size_t i = 0; i < poly.vertexIDX.size(); ++i) {
            size_t j = (i + 1) % poly.vertexIDX.size();
            int u = poly.vertexIDX[i];
            int v = poly.vertexIDX[j];
            const auto& pu = mesh.points[u];
            const auto& pv = mesh.points[v];

            // Собираем точки на этом ребре: начало, конец и точки из interiorSegments
            vector<pair<double, pair<double, double>>> points; // t, (x,y)
            points.emplace_back(0.0, make_pair(pu.x, pu.y));
            points.emplace_back(1.0, make_pair(pv.x, pv.y));

            for (const auto& seg : interiorSegments) {
                if (isPointOnSegment(seg.x0, seg.y0, pu.x, pu.y, pv.x, pv.y)) {
                    double t = projectOnEdge(seg.x0, seg.y0, pu.x, pu.y, pv.x, pv.y);
                    points.emplace_back(t, make_pair(seg.x0, seg.y0));
                }
                if (isPointOnSegment(seg.x1, seg.y1, pu.x, pu.y, pv.x, pv.y)) {
                    double t = projectOnEdge(seg.x1, seg.y1, pu.x, pu.y, pv.x, pv.y);
                    points.emplace_back(t, make_pair(seg.x1, seg.y1));
                }
            }

            // Сортировка по t и удаление близких точек
            sort(points.begin(), points.end(),
                 [](const auto& a, const auto& b) { return a.first < b.first; });
            vector<pair<double, pair<double, double>>> filtered;
            for (size_t k = 0; k < points.size(); ++k) {
                if (k == 0 || points[k].first - filtered.back().first > 1e-6) {
                    filtered.push_back(points[k]);
                }
            }

            // Создаём граничные отрезки
            for (size_t k = 0; k + 1 < filtered.size(); ++k) {
                double x0 = filtered[k].second.first, y0 = filtered[k].second.second;
                double x1 = filtered[k+1].second.first, y1 = filtered[k+1].second.second;
                if (hypot(x1 - x0, y1 - y0) > 1e-9) {
                    allSegments.emplace_back(x0, y0, x1, y1, SegmentType::BOUNDARY, -1);
                }
            }
        }
    }
}

vector<vector<Point2D>> ContourCreater::create(
    const vector<Segment2D>& interiorSegments,
    const MeshGraph& graph,
    const ParsedMesh& mesh,
    double height) {

    // 1. Добавляем граничные отрезки с разбиением
    vector<Segment2D> allSegments = interiorSegments;
    addBoundarySegments(mesh, interiorSegments, allSegments);

    if (allSegments.empty()) return {};

    // 2. Склеиваем близкие вершины и строим индекс
    unordered_map<PointKey, Point2D, PointKeyHash> pointMap;
    for (const auto& seg : allSegments) {
        PointKey k0{seg.x0, seg.y0};
        PointKey k1{seg.x1, seg.y1};
        if (pointMap.find(k0) == pointMap.end()) pointMap[k0] = {seg.x0, seg.y0};
        if (pointMap.find(k1) == pointMap.end()) pointMap[k1] = {seg.x1, seg.y1};
    }

    vector<Point2D> pts;
    pts.reserve(pointMap.size());
    unordered_map<PointKey, int, PointKeyHash> pointToIndex;
    for (const auto& entry : pointMap) {
        int idx = static_cast<int>(pts.size());
        pointToIndex[entry.first] = idx;
        pts.push_back(entry.second);
    }

    struct DirectedSegment {
        int startIdx, endIdx;
        int originalIdx; // индекс в allSegments
        bool used = false;
    };
    vector<DirectedSegment> dirSegs;
    dirSegs.reserve(allSegments.size());
    for (size_t i = 0; i < allSegments.size(); ++i) {
        const auto& seg = allSegments[i];
        int s = pointToIndex.at(PointKey{seg.x0, seg.y0});
        int e = pointToIndex.at(PointKey{seg.x1, seg.y1});
        if (s != e) {
            dirSegs.push_back({s, e, static_cast<int>(i), false});
        }
    }

    // Списки исходящих рёбер для каждой вершины
    vector<vector<int>> outEdges(pts.size());
    for (size_t i = 0; i < dirSegs.size(); ++i) {
        outEdges[dirSegs[i].startIdx].push_back(static_cast<int>(i));
    }

    // 4. Обход – выделение замкнутых ориентированных контуров
    vector<vector<Point2D>> contours;

    for (size_t startIdx = 0; startIdx < dirSegs.size(); ++startIdx) {
        if (dirSegs[startIdx].used) continue;

        vector<int> vertSequence;
        int startVert = dirSegs[startIdx].startIdx;
        int currVert = dirSegs[startIdx].endIdx;
        dirSegs[startIdx].used = true;
        vertSequence.push_back(startVert);
        vertSequence.push_back(currVert);

        while (currVert != startVert) {
            int nextEdge = -1;
            for (int e : outEdges[currVert]) {
                if (!dirSegs[e].used) {
                    nextEdge = e;
                    break;
                }
            }
            if (nextEdge == -1) break; // обрыв (не должно происходить)

            dirSegs[nextEdge].used = true;
            int nextVert = dirSegs[nextEdge].endIdx;
            vertSequence.push_back(nextVert);
            currVert = nextVert;
        }

        if (currVert == startVert && vertSequence.size() >= 3) {
            vector<Point2D> contour;
            contour.reserve(vertSequence.size());
            for (int vi : vertSequence) contour.push_back(pts[vi]);
            contours.push_back(move(contour));
        }
    }

    return contours;
}
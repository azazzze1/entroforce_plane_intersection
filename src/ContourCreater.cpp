#include "intersection/ContourCreater.hpp"
#include <algorithm>
#include <cmath>
#include <set>



std::vector<std::vector<Point2D>> ContourCreater::create(const std::vector<Segment2D>& rawSegments, const MeshGraph& graph, double height) {
    std::unordered_map<PointKey, std::vector<PointKey>, PointKeyHash> adj;
    
    // 1. Жёсткое приведение всех точек к сетке 1e-3
    auto snap = [](double v) { return std::round(v * 1000.0) / 1000.0; };

    for (const auto& seg : rawSegments) {
        PointKey p1{snap(seg.x0), snap(seg.y0)};
        PointKey p2{snap(seg.x1), snap(seg.y1)};
        adj[p1].push_back(p2);
        adj[p2].push_back(p1);
    }

    std::vector<std::vector<Point2D>> contours;
    std::set<std::pair<PointKey, PointKey>> usedEdges;
    auto edgeKey = [](const PointKey& a, const PointKey& b) {
        return (a < b) ? std::make_pair(a, b) : std::make_pair(b, a);
    };

    // 2. Обход по РЁБРАМ, а не по вершинам
    for (const auto& [start, neighbors] : adj) {
        for (const auto& next : neighbors) {
            auto ek = edgeKey(start, next);
            if (usedEdges.count(ek)) continue;

            std::vector<Point2D> contour;
            PointKey curr = start;
            PointKey prev = next;
            usedEdges.insert(ek);
            contour.push_back({curr.x, curr.y});

            while (true) {
                contour.push_back({prev.x, prev.y});
                const auto& nbs = adj[prev];
                if (nbs.empty()) break;

                // 3. Сортировка соседей по углу относительно входящего ребра
                // Гарантирует, что на стыках и границах мы не "перепрыгнем" через себя
                double inAngle = std::atan2(curr.y - prev.y, curr.x - prev.x);
                std::vector<std::pair<double, PointKey>> sorted;
                sorted.reserve(nbs.size());
                for (const auto& nb : nbs) {
                    double ang = std::atan2(nb.y - prev.y, nb.x - prev.x);
                    double rel = ang - inAngle;
                    while (rel <= -M_PI) rel += 2 * M_PI;
                    while (rel > M_PI) rel -= 2 * M_PI;
                    sorted.emplace_back(rel, nb);
                }
                std::sort(sorted.begin(), sorted.end());

                PointKey nextNb;
                bool found = false;
                for (const auto& [relAng, nb] : sorted) {
                    if (!usedEdges.count(edgeKey(prev, nb))) {
                        nextNb = nb;
                        found = true;
                        break;
                    }
                }

                if (!found) break; // Тупик или все рёбра использованы
                if (nextNb == start) {
                    contour.push_back({nextNb.x, nextNb.y});
                    break;
                }

                usedEdges.insert(edgeKey(prev, nextNb));
                curr = prev;
                prev = nextNb;
            }

            if (contour.size() >= 3) {
                // 4. Проверка ориентации: BELOW должен быть слева => CCW
                double area = 0;
                for (size_t i = 0; i < contour.size(); ++i) {
                    size_t j = (i + 1) % contour.size();
                    area += contour[i].x * contour[j].y - contour[j].x * contour[i].y;
                }
                if (area < 0) std::reverse(contour.begin(), contour.end());
                contours.push_back(std::move(contour));
            }
        }
    }

    return contours;
}

bool ContourCreater::isOrientedCorrect(const std::vector<Point2D>& contour, const MeshGraph& graph, double height) { return true; }
void ContourCreater::reverseContour(std::vector<Point2D>& contour) { std::reverse(contour.begin(), contour.end()); }
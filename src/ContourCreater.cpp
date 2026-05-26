#include "intersection/ContourCreater.hpp"

static bool isPointVisited(const std::vector<PointKey>& visited, const PointKey& p) {
    for (const auto& vp : visited) {
        if (vp == p) return true;
    }
    return false;
}

std::vector<std::vector<Point2D>> ContourCreater::create(const std::vector<Segment2D>& rawSegments, const MeshGraph& graph,double height){
    std::unordered_map<PointKey, std::vector<PointKey>, PointKeyHash> adj;

    auto addEdge = [&](double x1, double y1, double x2, double y2) {
        PointKey p1{x1, y1};
        PointKey p2{x2, y2};
        adj[p1].push_back(p2);
        adj[p2].push_back(p1);
    };

    for (const auto& seg : rawSegments) {
        addEdge(seg.x0, seg.y0, seg.x1, seg.y1);
    }

    std::vector<std::vector<Point2D>> contours;
    std::vector<PointKey> globalVisited;

    for (const auto& [startNode, _] : adj) {
        if (isPointVisited(globalVisited, startNode)) continue;
        
        std::vector<Point2D> contour;
        PointKey curr = startNode;
        PointKey prev = startNode; 
        
        while (true) {
            if (!isPointVisited(globalVisited, curr)) {
                globalVisited.push_back(curr);
            }
            contour.push_back({curr.x, curr.y});
            
            const auto& neighbors = adj[curr];
            PointKey next;
            bool found = false;
            
            for (const auto& nb : neighbors) {
                if (std::hypot(nb.x - prev.x, nb.y - prev.y) > 1e-6) {
                    next = nb;
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                break;
            }
            
            if (std::hypot(next.x - startNode.x, next.y - startNode.y) < 1e-6) {
                contour.push_back({next.x, next.y}); 
                break;
            }
            
            prev = curr;
            curr = next;
        }
        
        if (contour.size() >= 3) {
            contours.push_back(std::move(contour));
        }
    }

    return contours;
}
#include "mesh/MeshGraph.hpp"

bool MeshGraph::isPointInsidePolygon(double px, double py, const std::vector<int>& polyIDX, const std::vector<Point3D>& points){
    bool inside = false;
    int N = polyIDX.size();

    if (N < 3) return inside;

    for (int i = 0, j = N - 1; i < N; j=++i){
        double xi = points[polyIDX[i]].x;
        double yi = points[polyIDX[i]].y;
        double xj = points[polyIDX[j]].x;
        double yj = points[polyIDX[j]].y;

        bool intersect = ((yi > py) != (yj > py)) &&
                         (px < (xj - xi) * (py - yi) / (yj - yi) + xi);
        if (intersect) inside = !inside;
    }
    return inside; 
}

MeshGraph::MeshGraph(const CDT& cdt, const ParsedMesh& originalData) {
    vertices = originalData.points;

    if (originalData.polygons.empty()) {
        for (auto fit = cdt.finite_faces_begin(); fit != cdt.finite_faces_end(); ++fit) {
            faces.push_back({
                static_cast<int>(fit->vertex(0)->info().original_id),
                static_cast<int>(fit->vertex(1)->info().original_id),
                static_cast<int>(fit->vertex(2)->info().original_id)
            });
        }
    } else {
        const auto& outerPoly = originalData.polygons[0];
        
        for (auto fit = cdt.finite_faces_begin(); fit != cdt.finite_faces_end(); ++fit) {
            auto p0 = fit->vertex(0)->point();
            auto p1 = fit->vertex(1)->point();
            auto p2 = fit->vertex(2)->point();
            
            double bx = (p0.x() + p1.x() + p2.x()) / 3.0;
            double by = (p0.y() + p1.y() + p2.y()) / 3.0;

            if (!isPointInsidePolygon(bx, by, outerPoly.vertexIDX, vertices)) {
                continue;
            }

            bool inHole = false;
            for (size_t h = 1; h < originalData.polygons.size(); ++h) {
                if (isPointInsidePolygon(bx, by, originalData.polygons[h].vertexIDX, vertices)) {
                    inHole = true;
                    break;
                }
            }
            if (inHole) continue;

            faces.push_back({
                static_cast<int>(fit->vertex(0)->info().original_id),
                static_cast<int>(fit->vertex(1)->info().original_id),
                static_cast<int>(fit->vertex(2)->info().original_id)
            });
        }
    }

    std::set<std::pair<int, int>> uniqueEdges;
    for (const auto& f : faces) {
        uniqueEdges.emplace(std::min(f.v0, f.v1), std::max(f.v0, f.v1));
        uniqueEdges.emplace(std::min(f.v1, f.v2), std::max(f.v1, f.v2));
        uniqueEdges.emplace(std::min(f.v2, f.v0), std::max(f.v2, f.v0));
    }

    edges.reserve(uniqueEdges.size());
    for (const auto& e : uniqueEdges) {
        edges.push_back({e.first, e.second});
    }
}
#include "triangulation/Triangulator.hpp"

std::unique_ptr<CDT> Triangulator::triangulate(const ParsedMesh& mesh) const {
    if (mesh.points.empty()) {
        throw std::runtime_error("Triangulator: vertex list is empty.");
    }

    auto cdt = std::make_unique<CDT>();

    std::vector<std::pair<Point2, VertexInfo>> infoPoints;
    infoPoints.reserve(mesh.points.size());
    for(size_t i = 0; i < mesh.points.size(); ++i) {
        infoPoints.emplace_back(
            Point2(mesh.points[i].x, mesh.points[i].y),
            VertexInfo{static_cast<unsigned>(i), mesh.points[i].z}
        );
    }

    cdt->insert(infoPoints.begin(), infoPoints.end());

    std::vector<VertexHandle> handleID(mesh.points.size());
    for(auto vit = cdt->finite_vertices_begin(); vit != cdt->finite_vertices_end(); ++vit) {
        handleID[vit->info().original_id] = vit;
    }

    std::set<std::pair<int, int>> uniqueEdges;
    for(const auto& poly : mesh.polygons) {
        if (poly.vertexIDX.size() < 2) continue;

        for (size_t i = 0; i < poly.vertexIDX.size(); ++i) {
            int u = poly.vertexIDX[i];
            int v = poly.vertexIDX[(i + 1) % poly.vertexIDX.size()];

            if (u < 0 || u >= static_cast<int>(mesh.points.size()) || 
                v < 0 || v >= static_cast<int>(mesh.points.size())) {
                throw std::runtime_error("Triangulator: Invalid polygon vertex index");
            }
            
            if (u == v) continue;

            if (u > v) uniqueEdges.emplace(v, u);
            else uniqueEdges.emplace(u, v);
        }
    }

    for (const auto& [u, v] : uniqueEdges) {
        cdt->insert_constraint(handleID[u], handleID[v]);
    }

    return cdt;
}
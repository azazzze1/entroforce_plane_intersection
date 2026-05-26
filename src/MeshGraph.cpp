#include "mesh/MeshGraph.hpp"

MeshGraph::MeshGraph(const CDT& cdt, const ParsedMesh& originalData) {
    vertices = originalData.points;

    for (auto fit = cdt.finite_faces_begin(); fit != cdt.finite_faces_end(); ++fit) {
        faces.push_back({
            static_cast<int>(fit->vertex(0)->info().original_id),
            static_cast<int>(fit->vertex(1)->info().original_id),
            static_cast<int>(fit->vertex(2)->info().original_id)
        });
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

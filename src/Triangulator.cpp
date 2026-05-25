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

    for (const auto& poly : mesh.polygons) {
        if (poly.vertexIDX.size() < 3) continue;
        
        std::vector<Point2> contour;
        contour.reserve(poly.vertexIDX.size());
        for (int idx : poly.vertexIDX) {
            contour.emplace_back(mesh.points[idx].x, mesh.points[idx].y);
        }

        cdt->insert_constraint(contour.begin(), contour.end(), true);
    }

    CGAL::mark_domain_in_triangulation(*cdt, InDomainMap());

    std::vector<CDT::Face_handle> to_delete;
    for (auto f = cdt->finite_faces_begin(); f != cdt->finite_faces_end(); ++f)
    {
        if (!f->info())
            to_delete.push_back(f);
    }
    for (auto f : to_delete)
        cdt->delete_face(f);

    return cdt;
}
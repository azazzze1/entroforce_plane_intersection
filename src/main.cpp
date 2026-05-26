
#include "dataIO/MeshLoader.hpp"
#include "triangulation/Triangulator.hpp"
#include "mesh/MeshGraph.hpp"
#include "intersection/VertexClassifier.hpp"
#include "intersection/ContourCreater.hpp"
#include <fstream>


double snap(double v) { return std::round(v * 10000.0) / 10000.0; }

int main() {    
    MeshLoader loader;
    Triangulator triang;
    std::string filename = "../meshGenerator/mesh.txt";
    std::ifstream file(filename);
    if (!file.is_open()) { std::cerr << "Ошибка файла\n"; return 1; }
    ParsedMesh mesh = loader.load(file);
    file.close();
    auto cdt = triang.triangulate(mesh);
    MeshGraph graph(*cdt, mesh);

    double cut_height = 2.5; 
    auto segments = VertexClassifier::extractSegments(graph, cut_height);

  
    for (size_t p = 0; p < mesh.polygons.size(); ++p) {
        const auto & poly = mesh.polygons[p];
        for (size_t i = 0; i < poly.vertexIDX.size(); ++i) {
            int u = poly.vertexIDX[i];
            int v = poly.vertexIDX[(i + 1) % poly.vertexIDX.size()];
            
            segments.push_back({
                 snap(graph.vertices[u].x), snap(graph.vertices[u].y),
                 snap(graph.vertices[v].x), snap(graph.vertices[v].y)
            });
        }
    }

    auto contours = ContourCreater::create(segments, graph, cut_height);

    std::cout << "Найдено замкнутых контуров: " << contours.size() << "\n";

    std::ofstream out("../meshGenerator/contours_final.txt");
    if (out.is_open()) {
        for (const auto& contour : contours) {
            for (const auto& p : contour) {
                out << p.x << " " << p.y << "\n";
            }
            out << "\n";
        }
        out.close();
    }

    return 0;
}
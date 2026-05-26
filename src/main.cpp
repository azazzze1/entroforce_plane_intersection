
#include "dataIO/MeshLoader.hpp"
#include "triangulation/Triangulator.hpp"
#include "mesh/MeshGraph.hpp"
#include "intersection/VertexClassifier.hpp"
#include "intersection/ContourCreater.hpp"
#include <fstream>

int main()
{
    MeshLoader loader;
    Triangulator triang;

    std::string filename = "../meshGenerator/mesh.txt";

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка: не удалось открыть файл " << filename << "\n";
        return 1;
    }

    ParsedMesh mesh = loader.load(file);
    file.close();

    auto cdt = triang.triangulate(mesh);

    MeshGraph graph(*cdt, mesh);

    graph.exportGraphToTXT("../meshGenerator/filtered_triangulation.txt");
    
    double cut_height = 2.5; 
    auto segments = VertexClassifier::extractSegments(graph, cut_height);

    for (size_t p = 0; p < mesh.polygons.size(); ++p) {
        const auto& poly = mesh.polygons[p];
        for (size_t i = 0; i < poly.vertexIDX.size(); ++i) {
            int u = poly.vertexIDX[i];
            int v = poly.vertexIDX[(i + 1) % poly.vertexIDX.size()];
            
            segments.push_back({
                graph.vertices[u].x, graph.vertices[u].y,
                graph.vertices[v].x, graph.vertices[v].y
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

#include "dataIO/MeshLoader.hpp"
#include "triangulation/Triangulator.hpp"
#include "mesh/MeshGraph.hpp"
#include "intersection/VertexClassifier.hpp"
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
    auto cdt = triang.triangulate(mesh);

    MeshGraph graph(*cdt, mesh);

    graph.exportGraphToTXT("../meshGenerator/filtered_triangulation.txt");
    

    double cut_height = 2.5; 
    auto segments = VertexClassifier::extractSegments(graph, cut_height);

    std::cout << " Найдено отрезков пересечения: " << segments.size() << "\n";

    std::ofstream out("../meshGenerator/contours.txt");
    if (out.is_open()) {
        for (const auto& seg : segments) {
            out << seg.x0 << " " << seg.y0 << "\n";
            out << seg.x1 << " " << seg.y1 << "\n\n"; 
        }
        out.close();
    }

    file.close();

    return 0;
}
#include "dataIO/MeshLoader.hpp"
#include "triangulation/Triangulator.hpp"
#include "mesh/MeshGraph.hpp"
#include "intersection/VertexClassifier.hpp"
#include "intersection/ContourCreater.hpp"
#include <fstream>
#include <iostream>

int main() {
    MeshLoader loader;
    Triangulator triang;
    std::string filename = "../meshGenerator/mesh.txt";
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка открытия файла\n";
        return 1;
    }
    ParsedMesh mesh = loader.load(file);
    file.close();

    auto cdt = triang.triangulate(mesh);
    MeshGraph graph(*cdt, mesh);

    double cut_height = 2.5;
    auto interiorSegments = VertexClassifier::extractSegments(graph, cut_height);

    // Передаём в ContourCreater и внутренние отрезки, и параметры области
    auto contours = ContourCreater::create(interiorSegments, graph, mesh, cut_height);

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
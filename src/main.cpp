#include "dataIO/MeshLoader.hpp"
#include "triangulation/Triangulator.hpp"
#include "mesh/MeshGraph.hpp"
#include "intersection/VertexClassifier.hpp"
#include "intersection/ContourCreater.hpp"
#include <fstream>
#include <iostream>

void printUsage(const char* progName) {
    std::cerr << "Использование:\n";
    std::cerr << "\t" << progName << " [filename] [heigth]\n";
    std::cerr << "Параметры:\n";
    std::cerr << "\tfilename -- путь к mesh.txt (по умолчанию: ../meshGenerator/mesh.txt)\n";
    std::cerr << "\theight -- высота плоскости сечения (по умолчанию: 2.5)\n";
}

int main(int argc, char* argv[]) {
    std::string filename = "../meshGenerator/mesh.txt";
    double cut_height = 2.5;

    if (argc >= 2) {
        std::string arg1 = argv[1];
        if (arg1 == "-h" || arg1 == "--help") {
            printUsage(argv[0]);
            return 0;
        }
        filename = arg1;
    }
    if (argc >= 3) {
        try {
            cut_height = std::stod(argv[2]);
        } catch (const std::exception& e) {
            std::cerr << "Ошибка: некорректное значение высоты '" << argv[2] << "'\n";
            printUsage(argv[0]);
            return 1;
        }
    }

    MeshLoader loader;
    Triangulator triang;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка открытия файла\n";
        return 1;
    }
    ParsedMesh mesh = loader.load(file);
    file.close();

    auto cdt = triang.triangulate(mesh);
    MeshGraph graph(*cdt, mesh);

    auto segments = VertexClassifier::extractSegments(graph, cut_height);

    auto contours = ContourCreater::create(segments, mesh);

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
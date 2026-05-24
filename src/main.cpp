
#include "dataIO/MeshLoader.hpp"
#include "triangulation/Triangulator.hpp"
#include "mesh/MeshGraph.hpp"
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

    file.close();

    return 0;
}
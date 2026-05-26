#ifndef MESH_GRAPH_HPP
#define MESH_GRAPH_HPP

#include <vector>
#include <string>
#include "dataIO/MeshLoader.hpp" 
#include "mesh/MeshGraph.hpp"
#include "triangulation/Triangulator.hpp"
#include <algorithm>
#include <set>
#include <cmath>
#include <queue>

// Класс для перевода CDT графа в граф, с которым можно работать через встроенные библиотеки 
class MeshGraph{
public:
    std::vector<Point3D> vertices;
    std::vector<Face> faces;
    std::vector<Edge> edges;
    
    /**
     * Конструктор: преобразует CDT и исходные данные в плоский граф
     * Вход:
     *   cdt - триангуляция CGAL
     *   originalData - исходные точки и полигоны для восстановления индексов вершин
     */
    MeshGraph(const CDT& cdt, const ParsedMesh& originalData);
};

#endif
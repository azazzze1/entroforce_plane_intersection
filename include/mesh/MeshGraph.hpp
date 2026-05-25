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

class MeshGraph{
public:
    std::vector<Point3D> vertices;
    std::vector<Face> faces;
    std::vector<Edge> edges;
    
    MeshGraph(const CDT& cdt, const ParsedMesh& originalData);
    bool exportGraphToTXT(const std::string& filename) const;


private: 
    bool isPointInsidePolygon(double px, double py, const std::vector<int>& polyIDX, const std::vector<Point3D>& points);
 
};

#endif
#ifndef CONTOURCREATER_HPP
#define CONTOURCREATER_HPP

#include <vector>
#include "dataIO/Types.hpp"
#include "mesh/MeshGraph.hpp"
#include "dataIO/MeshLoader.hpp"

#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <utility> 


class ContourCreater {
public:
    static std::vector<std::vector<Point2D>> create(const std::vector<Segment2D>& segments, const ParsedMesh& mesh) ;

private:
    static bool isPointOnSegment(double px, double py, double ax, double ay, double bx, double by);
    static double projectOnEdge(double px, double py, double ax, double ay, double bx, double by);
    static void addBoundarySegments(const ParsedMesh& mesh, const std::vector<Segment2D>& interiorSegments, std::vector<Segment2D>& allSegments);
};

#endif
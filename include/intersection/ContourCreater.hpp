// ContourCreater.hpp
#ifndef CONTOURCREATER_HPP
#define CONTOURCREATER_HPP

#include <vector>
#include "dataIO/Types.hpp"
#include "mesh/MeshGraph.hpp"
#include "dataIO/MeshLoader.hpp"

class ContourCreater {
public:

    static std::vector<std::vector<Point2D>> create(const std::vector<Segment2D>& interiorSegments, const MeshGraph& graph, const ParsedMesh& mesh, double height);

private:
    struct PointKey {
        double x, y;
        bool operator==(const PointKey& other) const {
            return std::hypot(x - other.x, y - other.y) < 1e-6;
        }

        bool operator<(const PointKey& other) const {
            if (std::abs(x - other.x) > 1e-6) return x < other.x;
            return y < other.y;
        }
    };
    
    struct PointKeyHash {
        size_t operator()(const PointKey& p) const {
            long long ix = static_cast<long long>(std::round(p.x * 1e6));
            long long iy = static_cast<long long>(std::round(p.y * 1e6));
            return std::hash<long long>{}(ix) ^ (std::hash<long long>{}(iy) << 1);
        }
    };

    static bool isPointOnSegment(double px, double py, double ax, double ay, double bx, double by);
    static double projectOnEdge(double px, double py, double ax, double ay, double bx, double by);
    static void addBoundarySegments(const ParsedMesh& mesh,
                                    const std::vector<Segment2D>& interiorSegments,
                                    std::vector<Segment2D>& allSegments);
};

#endif
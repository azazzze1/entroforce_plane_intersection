#ifndef COUNTOR_CREATOR_HPP
#define COUNTOR_CREATOR_HPP

#include "intersection/VertexClassifier.hpp"
#include "mesh/MeshGraph.hpp"
#include <vector>
#include <unordered_map>
#include <cmath>
#include <functional>

struct PointKey {
    double x, y;
    
    bool operator==(const PointKey& other) const {
        return std::hypot(x - other.x, y - other.y) < 1e-6;
    }
    
    bool operator<(const PointKey& other) const {
        return x < other.x || (x == other.x && y < other.y);
    }
};

struct PointKeyHash {
    size_t operator()(const PointKey& p) const {
        long long ix = static_cast<long long>(std::round(p.x * 10000));
        long long iy = static_cast<long long>(std::round(p.y * 10000));
        return std::hash<long long>{}(ix) ^ (std::hash<long long>{}(iy) << 1);
    }
};

class ContourCreater{
public: 
    static std::vector<std::vector<Point2D>> create(const std::vector<Segment2D>& rawSegments, const MeshGraph& graph, double height);

private:
    bool isOrientedCorrect(const std::vector<Point2D>& contour, const MeshGraph& graph, double height);
    static void reverseContour(std::vector<Point2D>& contour);
 };

#endif
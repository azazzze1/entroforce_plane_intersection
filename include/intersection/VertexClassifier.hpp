#ifndef VERTEX_CLASSIFIER_HPP
#define VERTEX_CLASSIFIER_HPP

#include <vector>
#include "dataIO/Types.hpp"
#include "mesh/MeshGraph.hpp"
#include <unordered_map>
#include <cmath>
#include <algorithm>

struct EdgeKey {
    int v0, v1;
    bool operator==(const EdgeKey& o) const {
        return (v0 == o.v0 && v1 == o.v1) || (v0 == o.v1 && v1 == o.v0);
    }
};

struct EdgeKeyHash {
    size_t operator()(const EdgeKey& e) const {
        int a = std::min(e.v0, e.v1);
        int b = std::max(e.v0, e.v1);
        return std::hash<int>{}(a) ^ (std::hash<int>{}(b) << 1);
    }
};

class VertexClassifier {
private:
    static constexpr double EPS = 1e-6;
    static std::pair<double, double> interpolate(const Point3D& v0, const Point3D& v1, double height);
    static bool isLeft(double x0, double y0, double x1, double y1, double x2, double y2);

public:
    static std::vector<Segment2D> extractSegments(const MeshGraph& graph, double height);
};

#endif
// VertexClassifier.hpp
#ifndef VERTEX_CLASSIFIER_HPP
#define VERTEX_CLASSIFIER_HPP

#include <vector>
#include "dataIO/Types.hpp"
#include "mesh/MeshGraph.hpp"

class VertexClassifier {
private:
    static constexpr double EPS = 1e-6;
    static std::pair<double, double> interpolate(const Point3D& v0, const Point3D& v1, double height);
    static bool isLeft(double fromX, double fromY, double toX, double toY, double pointX, double pointY);

public:
    static std::vector<Segment2D> extractSegments(const MeshGraph& graph, double height);
};

#endif
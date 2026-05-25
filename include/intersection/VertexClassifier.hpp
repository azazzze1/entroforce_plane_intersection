#ifndef VETREX_INTERSECTION_HPP
#define VETREX_INTERSECTION_HPP

#include <vector>
#include "mesh/MeshGraph.hpp"

class VertexClassifier{
private:
    static constexpr double EPS = 1e-6;
    static std::pair<double, double> interpolate(const Point3D& v0, const Point3D& v1, double height);
    
public: 
    static std::vector<Segment2D> extractSegments(const MeshGraph& graph, double height);
};


#endif
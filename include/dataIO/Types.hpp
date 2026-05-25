#ifndef TYPES_HPP
#define TYPES_HPP

struct Point2D { double x, y; };
struct Point3D { double x, y, z; };
struct Edge { int v0, v1; };
struct Face { int v0, v1, v2; };

struct Segment2D {
    double x0, y0, x1, y1;

    Segment2D(double x0_, double y0_, double x1_, double y1_) 
        : x0(x0_), y0(y0_), x1(x1_), y1(y1_) {}
};

struct Polygon {
    std::vector<int> vertexIDX;
};

enum class FaceType { INSIDE, OUTSIDE, HOLE };

enum class VertexState { BELOW, ON, ABOVE };

struct ParsedMesh {
    std::vector<Point3D> points;
    std::vector<Polygon> polygons; 
};

#endif
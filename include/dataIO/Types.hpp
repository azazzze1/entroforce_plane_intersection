#ifndef TYPES_HPP
#define TYPES_HPP

constexpr double EPS = 1e-9;
    
struct Point2D { double x, y; };
struct Point3D { double x, y, z; };
struct Edge { int v0, v1; };
struct Face { int v0, v1, v2; };

struct Polygon {
    std::vector<int> vertexIDX;
};

enum class FaceType { INSIDE, OUTSIDE, HOLE };

struct ParsedMesh {
    std::vector<Point3D> points;
    std::vector<Polygon> polygons; 
};

#endif
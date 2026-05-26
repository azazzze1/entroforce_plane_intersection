#ifndef TYPES_HPP
#define TYPES_HPP

#include <vector>
#include <cmath>

struct Point2D { double x, y; };
struct Point3D { double x, y, z; };
struct Edge { int v0, v1; };
struct Face { int v0, v1, v2; };

struct Polygon {
    std::vector<int> vertexIDX;
};

struct ParsedMesh {
    std::vector<Point3D> points;
    std::vector<Polygon> polygons;
};

enum class SegmentType { INTERIOR, BOUNDARY };
enum class FaceType { INSIDE, OUTSIDE, HOLE };
enum class VertexState { BELOW, ON, ABOVE };

struct Segment2D {
    double x0, y0, x1, y1;
    SegmentType type = SegmentType::INTERIOR;
    int faceIndex = -1;

    Segment2D(double x0_, double y0_, double x1_, double y1_,
              SegmentType t = SegmentType::INTERIOR, int fi = -1)
        : x0(x0_), y0(y0_), x1(x1_), y1(y1_), type(t), faceIndex(fi) {}
};

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

#endif
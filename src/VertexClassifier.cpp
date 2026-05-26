#include "intersection/VertexClassifier.hpp"

std::pair<double, double> VertexClassifier::interpolate(const Point3D& v0, const Point3D& v1, double height){
    double diff = v1.z - v0.z;
    if (std::abs(diff) < 1e-12){
        return {(v0.x + v1.x) / 2, (v0.y + v1.y) / 2};
    }
    double t = (height - v0.z) / diff;
    return {v0.x + t * (v1.x - v0.x), v0.y + t * (v1.y - v0.y)};
}

std::vector<Segment2D> VertexClassifier::extractSegments(const MeshGraph& graph, double height) {
    std::vector<Segment2D> segments;
    segments.reserve(graph.faces.size() / 2);

    std::vector<VertexState> states(graph.vertices.size());
    for (int i = 0; i < graph.vertices.size(); ++i) {
        double diff = graph.vertices[i].z - height;
        if (std::abs(diff) <= EPS) states[i] = VertexState::ON;
        else if (diff < 0.0) states[i] = VertexState::BELOW;
        else states[i] = VertexState::ABOVE;
    }

    for (const auto& f : graph.faces) {
        VertexState s[3] = {states[f.v0], states[f.v1], states[f.v2]};
        const Point3D* v[3] = {&graph.vertices[f.v0], &graph.vertices[f.v1], &graph.vertices[f.v2]};

        int belowCount = 0, aboveCount = 0, onCount = 0;
        for (int i = 0; i < 3; ++i) {
            if (s[i] == VertexState::BELOW) ++belowCount;
            else if (s[i] == VertexState::ABOVE) ++aboveCount;
            else ++onCount;
        }

        if (belowCount == 3 || aboveCount == 3) continue;

        if (onCount == 2) {
            if (s[0] == VertexState::ON && s[1] == VertexState::ON && s[2] == VertexState::BELOW) 
                segments.push_back({v[0]->x, v[0]->y, v[1]->x, v[1]->y});
            
            else if (s[0] == VertexState::ON && s[2] == VertexState::ON && s[1] == VertexState::BELOW) 
                segments.push_back({v[0]->x, v[0]->y, v[2]->x, v[2]->y});
            
            else if (s[1] == VertexState::ON && s[2] == VertexState::ON && s[0] == VertexState::BELOW)
                segments.push_back({v[1]->x, v[1]->y, v[2]->x, v[2]->y});
        }

        if (onCount == 1 && belowCount == 1 && aboveCount == 1) {
            int onIdx = -1, belowIdx = -1, aboveIdx = -1;
            for (int i = 0; i < 3; ++i) {
                if (s[i] == VertexState::ON) onIdx = i;
                else if (s[i] == VertexState::BELOW) belowIdx = i;
                else aboveIdx = i;
            }
            auto [ix, iy] = interpolate(*v[belowIdx], *v[aboveIdx], height);
            segments.push_back({v[onIdx]->x, v[onIdx]->y, ix, iy});
        }

         if (onCount == 0) {
            int singleIDX = -1;
            for (int i = 0; i < 3; ++i) {
                if ((belowCount == 1 && s[i] == VertexState::BELOW) ||
                    (aboveCount == 1 && s[i] == VertexState::ABOVE)) {
                    singleIDX = i;
                    break;
                }
            }
            int e0 = (singleIDX + 1) % 3;
            int e1 = (singleIDX + 2) % 3;
            auto [x1, y1] = interpolate(*v[singleIDX], *v[e0], height);
            auto [x2, y2] = interpolate(*v[singleIDX], *v[e1], height);
            segments.push_back({x1, y1, x2, y2});
        }
    }

    return segments;
}

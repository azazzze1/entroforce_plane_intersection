#include "dataIO/MeshLoader.hpp"

ParsedMesh MeshLoader::load(std::istream& in) const{
    ParsedMesh data;

    int N = 0, M = 0;
    in >> N >> M;

    if(N < 1 || M < 0) throw std::runtime_error("Invalid points count! " + std::to_string(N) + " " + std::to_string(M));

    data.points.reserve(N);
    data.polygons.reserve(M);

    for(int i = 0; i < N; ++i){
        Point3D tmpPoint;
        in >> tmpPoint.x >> tmpPoint.y >> tmpPoint.z; 
        if(!in) throw std::runtime_error("Failed to read point " + std::to_string(i));

        data.points.push_back(tmpPoint);
    }

    for(int i = 0; i < M; ++i) {
        int polyLen = 0;
        in >> polyLen;
        if (polyLen < 3)
            throw std::runtime_error("Invalid polygon length at index " + std::to_string(i));
        
        Polygon poly;
        poly.vertexIDX.reserve(polyLen);
        for (int j = 0; j < polyLen; ++j) {
            int idx = 0;
            in >> idx;
            if (idx < 0 || idx >= N)
                throw std::runtime_error("Invalid vertex index in polygon " + std::to_string(i));
            poly.vertexIDX.push_back(idx);
        }
        data.polygons.push_back(poly);
    }

    return data;
}
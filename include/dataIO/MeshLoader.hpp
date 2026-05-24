#ifndef MESH_LOADER_HPP
#define MESH_LOADER_HPP

#include <iostream>
#include <string>
#include <istream>
#include <vector>
#include <stdexcept>
#include <limits>
#include "Types.hpp"

class MeshLoader{
public:
    ParsedMesh load (std::istream& in) const;
};

#endif 

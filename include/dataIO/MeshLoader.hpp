#ifndef MESH_LOADER_HPP
#define MESH_LOADER_HPP

#include <iostream>
#include <string>
#include <istream>
#include <vector>
#include <stdexcept>
#include <limits>
#include "Types.hpp"

// Класс для обработки входных данных
class MeshLoader{
public:
    /**
     * Загружает данные точек и ограничений из текстового потока.
     * Вход: in -- поток (формат: N M, затем N строк точек, затем M полигонов).
     * Выход: структура ParsedMesh с обработанными данными.
     */
    ParsedMesh load (std::istream& in) const;
};

#endif 

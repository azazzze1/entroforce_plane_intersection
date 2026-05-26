#ifndef VERTEX_CLASSIFIER_HPP
#define VERTEX_CLASSIFIER_HPP

#include <vector>
#include "dataIO/Types.hpp"
#include "mesh/MeshGraph.hpp"
#include <unordered_map>
#include <algorithm>

class VertexClassifier {
private:
    static constexpr double EPS = 1e-6;
    /**
     * Линейная интерполяция точки пересечения отрезка [v0, v1] с плоскостью Z = height.
     * Вход: 
     *   v0, v1 - точки отрезка
     *   height - высота плоскости
     * Выход: координаты (x, y) точки пересечения 
     *
     * Используется в случае, когда плоскость пересекает треугольник на ребре.
     */
    static std::pair<double, double> interpolate(const Point3D& v0, const Point3D& v1, double height);

    /**
     * Проверка ориентации: лежит ли точка (x2,y2) слева от вектора (x0,y0)->(x1,y1).
     * Вход:
     *   x0, y0, x1, y1, x2, y2 - координаты соответсвующихх точек
     * Выход: true, если точка слева
     *
     * Проверка через косое произведение.
     */
    static bool isLeft(double x0, double y0, double x1, double y1, double x2, double y2);

public:
    /**
     * Генерирует 2D-сегменты, соответствующие линии пересечения поверхности с плоскостью
     * Вход:
     *   graph - информация о триангуляции
     *   height - высота плоскости
     * Выход: вектор сегментов с типами и индексами породивших граней.
     */
    static std::vector<Segment2D> extractSegments(const MeshGraph& graph, double height);
};

#endif
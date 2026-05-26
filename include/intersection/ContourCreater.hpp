#ifndef CONTOURCREATER_HPP
#define CONTOURCREATER_HPP

#include <vector>
#include "dataIO/Types.hpp"
#include "mesh/MeshGraph.hpp"
#include "dataIO/MeshLoader.hpp"

#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <utility> 


class ContourCreater {
public:
    /**
     * Объединение контуров и составление вектора контуров
     * Вход: 
     *   segments - все сегменты
     *   mesh - информация о триангуляции
     * Выход: вектор контуров 
     *
     * Объединяет внутренние сегменты и сегменты на границе. Строит граф смежности уникальных точек. Ищет замкнутые линии.  
     */
    static std::vector<std::vector<Point2D>> create(const std::vector<Segment2D>& segments, const ParsedMesh& mesh) ;

private:
    /**
     * Проверка: лежит ли точка (px,py) на отрезке [ax,ay]-[bx,by].
     * Вход: 
     *   px, py - координаты точки для проверки
     *   ax, ay, bx, by - координаты отрезка
     * Выход: true - если лежит, false - если нет
     *
     * Используется, чтобы определить, касается ли точка границы
     */
    static bool isPointOnSegment(double px, double py, double ax, double ay, double bx, double by);

    /**
     * Проекция точки на отрезок в параметрической форме t ∈ [0, 1].
     * Вход: аналогично isPointOnSegment
     * Выход: параметр t, ограниченный диапазоном [0.0, 1.0].
     *
     * Используется для разбиения ребра на части в addBoundarySegments
     */
    static double projectOnEdge(double px, double py, double ax, double ay, double bx, double by);

    /**
     * Добавляет сегменты границ домена в общий граф.
     * Вход: 
     *   mesh - информация о триангуляции
     *   interiorSegments - внутренние сегменты
     *   allSegments - все сегменты
     * 
     * Используется, чтобы разбить границу в точке соприкосновения с внутренним сегментом.
     * Необходимо, чтобы замкнуть контур, если он касается границы в разных местах  
     */
    static void addBoundarySegments(const ParsedMesh& mesh, const std::vector<Segment2D>& interiorSegments, std::vector<Segment2D>& allSegments);
};

#endif
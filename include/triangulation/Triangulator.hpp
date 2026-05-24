#ifndef TRIANGULATOR_HPP
#define TRIANGULATOR_HPP

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Constrained_triangulation_face_base_2.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>

#include "dataIO/MeshLoader.hpp"
#include <memory>  
#include <vector>
#include <stdexcept>
#include <algorithm>


using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;

struct VertexInfo {
    unsigned original_id;
    double z;
};

using Vb  = CGAL::Triangulation_vertex_base_with_info_2<VertexInfo, Kernel>;
using Fb  = CGAL::Constrained_triangulation_face_base_2<Kernel>;
using Tds = CGAL::Triangulation_data_structure_2<Vb, Fb>;

using CDT = CGAL::Constrained_Delaunay_triangulation_2<Kernel, Tds>;
using Point2 = Kernel::Point_2;
using Segment2 = Kernel::Segment_2;

using VertexHandle = CDT::Vertex_handle;


class Triangulator {
public:
    std::unique_ptr<CDT> triangulate(const ParsedMesh& mesh) const;
};

#endif
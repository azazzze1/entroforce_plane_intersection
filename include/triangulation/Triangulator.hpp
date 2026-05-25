#ifndef TRIANGULATOR_HPP
#define TRIANGULATOR_HPP

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h> 
#include <CGAL/Constrained_triangulation_face_base_2.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/mark_domain_in_triangulation.h> 


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

using Vb = CGAL::Triangulation_vertex_base_with_info_2<VertexInfo, Kernel>;
using Fb_base = CGAL::Constrained_triangulation_face_base_2<Kernel>;
using Fb = CGAL::Triangulation_face_base_with_info_2<bool, Kernel, Fb_base>;
using Tds = CGAL::Triangulation_data_structure_2<Vb, Fb>;

using CDT = CGAL::Constrained_Delaunay_triangulation_2<Kernel, Tds>;
using Point2 = Kernel::Point_2;
using Segment2 = Kernel::Segment_2;

using VertexHandle = CDT::Vertex_handle;

struct InDomainMap{
    typedef bool value_type;
    typedef bool& reference;
    typedef CDT::Face_handle key_type;
    typedef boost::read_write_property_map_tag  category;

    friend reference get(const InDomainMap&, key_type f) { return f->info(); }
    friend void put(const InDomainMap&, key_type f, value_type v) { f->info() = v; }
};

class Triangulator {
public:
    std::unique_ptr<CDT> triangulate(const ParsedMesh& mesh) const;
};

#endif
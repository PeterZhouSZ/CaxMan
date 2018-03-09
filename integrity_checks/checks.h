#ifndef CHECKS_H
#define CHECKS_H

#include "imatistl.h"

#include "caxlib/io/read_ANN.h"

#include <vector>

using namespace T_MESH;

int annotate_boundaries(IMATI_STL::TriMesh &tin, std::vector<caxlib::TriangleAnnotations> &lt_ann);

int annotate_no_manifold (IMATI_STL::TriMesh &tin, std::vector<caxlib::TriangleAnnotations> &lt_ann);

int annotate_inconsistent_normals (IMATI_STL::TriMesh &tin, std::vector<caxlib::TriangleAnnotations> &lt_ann);

int annotate_intersections (IMATI_STL::TriMesh &tin, std::vector<caxlib::TriangleAnnotations> &lt_ann);


#include "checks.cpp"

#endif // CHECKS_H

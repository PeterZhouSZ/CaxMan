#ifndef FILTER_PLC_H
#define FILTER_PLC_H

#include <cinolib/meshes/meshes.h>

void filter_PLC_by_label(const cinolib::Trimesh<> & plc,
                              int                   legal_flags,
                              std::vector<double> & coords,
                              std::vector<uint>   & tris);


cinolib::Trimesh<> filter_PLC_by_label(const cinolib::Trimesh<> & plc,
                                             int                   legal_labels);

#endif // FILTER_PLC_H

#ifndef SEGMENTATION_H
#define SEGMENTATION_H

#include <cinolib/meshes/meshes.h>

void detect_tiny_charts(      cinolib::Trimesh<>               & plc,
                        const double                             area_thresh,
                              std::vector< std::vector<uint> > & tiny_charts);

#endif // SEGMENTATION_H

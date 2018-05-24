#ifndef CLI_TO_VOL_H
#define CLI_TO_VOL_H

#include <cinolib/meshes/meshes.h>
#include <vector>

cinolib::Trimesh<> cli2PLC(const char   * filename,
                           const double   hatch_thickness);


void cli2PLC(const char          * filename,
             const double          hatch_thickness,
             std::vector<double> & coords,
             std::vector<uint>   & tris,
             std::vector<int>    & labels);


cinolib::Tetmesh<> PLC2tets(cinolib::Trimesh<> & PLC);

#endif // CLI_TO_VOL_H

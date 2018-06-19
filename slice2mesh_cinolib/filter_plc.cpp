#include "filter_plc.h"
#include "common.h"


cinolib::Trimesh<> filter_PLC_by_label(const cinolib::Trimesh<> & plc,
                                             int                   legal_labels)
{
    std::vector<double> coords;
    std::vector<uint>   tris;
    filter_PLC_by_label(plc, legal_labels, coords, tris);
    return cinolib::Trimesh<>(coords, tris);
}



void filter_PLC_by_label(const cinolib::Trimesh<>  & plc,
                               int                   legal_labels,
                               std::vector<double> & coords,
                               std::vector<uint>   & tris)
{
    assert(tris.empty() && coords.empty());

    for(uint pid=0; pid<plc.num_polys(); ++pid)
    {
        if (plc.poly_data(pid).label & legal_labels)
        {
            tris.push_back(plc.poly_vert_id(pid,0));
            tris.push_back(plc.poly_vert_id(pid,1));
            tris.push_back(plc.poly_vert_id(pid,2));
        }
    }

    // TODO: remove unreferenced vertices (if any) and re-index the whole PLC!!!!
    coords = serialized_xyz_from_vec3d(plc.vector_verts());
}

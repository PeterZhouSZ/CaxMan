#include "segmentation.h"
#include "filter_plc.h"
#include "common.h"
#include "tiny_chart_helper.h"

#include <cinolib/bfs.h>

using namespace cinolib;

void detect_connected_components(const Trimesh<> & plc, const int label, std::vector< std::vector<uint> > & ccs)
{
    // mask all the triangles having different label...
    std::vector<bool> mask(plc.num_polys());
    for(uint fid=0; fid<plc.num_polys(); ++fid) mask.at(fid) = (plc.poly_data(fid).label != label);

    // find a legal seed (any triangle with the right label)
    uint seed = 0;
    while (seed < plc.num_polys() && mask.at(seed)) ++seed;
    assert(seed < plc.num_polys());

    do
    {
        std::unordered_set<uint> cc;
        bfs_on_dual(plc, seed, mask, cc);

        ccs.push_back(std::vector<uint>(cc.begin(), cc.end()));
        for(uint fid : cc) mask.at(fid) = true; // mask the visited triangles

        seed = 0;
        while (seed < plc.num_polys() && mask.at(seed)) ++seed; // get a new legal seed, if any
    }
    while (seed < plc.num_polys());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void detect_tiny_charts(       Trimesh<>                 & plc,
                        const double                             area_thresh,
                              std::vector< std::vector<uint> > & tiny_charts)
{
    std::vector< std::vector<uint> > ccs;
    detect_connected_components(plc, SRF_FACE_UP,   ccs);
    detect_connected_components(plc, SRF_FACE_DOWN, ccs);

    std::vector<uint> bad_stuff;

    for(auto cc : ccs)
    {
        double area = 0.0;
        for(uint fid : cc) area += plc.poly_area(fid);
        assert(cc.size() > 0);

        if (area < area_thresh)
        {
            std::cout << "tiny protuberance found! (" << cc.size() << " tris - area: " << area << ")" << std::endl;

            tiny_charts.push_back(cc);

            std::vector<uint> tmp;
            for(uint fid : cc)
            {
                tmp.push_back(plc.poly_vert_id(fid,0));
                tmp.push_back(plc.poly_vert_id(fid,1));
                tmp.push_back(plc.poly_vert_id(fid,2));
            }
            std::copy(tmp.begin(), tmp.end(), std::back_inserter(bad_stuff));

            TinyChartHelper helper(plc, cc);
            plc = helper.push_chart_inside_PLC();
        }
    }

    Trimesh<> m(serialized_xyz_from_vec3d(plc.vector_verts()), bad_stuff);
    m.save("/Users/cino/Desktop/bad_stuff.off");
}

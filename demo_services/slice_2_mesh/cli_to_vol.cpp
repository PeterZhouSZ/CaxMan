#include "cli_to_vol.h"
#include "sliced_obj.h"
#include "edge_processing.h"
#include "triangulate_quad.h"
#include "cli_to_vol_debug.h"
#include "common.h"

#include <cinolib/tetgen_wrap.h>
#include <cinolib/triangle_wrap.h>
#include <cinolib/meshes/meshes.h>

using namespace std;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

SlicedObj                obj;
vector<V_data>           v_list;
vector< vector<E_data> > e_list;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void initialize();
void edge_wise_intersections();
void lift_unlifted_edge_extrema();
void order_split_points();
std::vector<int> order_split_points(const int vid_beg, const int vid_end, const std::set<int> & splits);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void mesh_vertical  (std::vector<uint> & tris, std::vector<int> & flags);
void mesh_horizontal(std::vector<uint> & tris, std::vector<int> &flags);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

cinolib::Trimesh<> cli2PLC(const char   * filename,
                           const double   hatch_thickness)
{
    std::vector<double> coords;
    std::vector<uint>   tris;
    std::vector<int>    labels;

    cli2PLC(filename, hatch_thickness, coords, tris, labels);

    cinolib::Trimesh<> plc(coords, tris);
    for(uint fid=0; fid<plc.num_polys(); ++fid) plc.poly_data(fid).label = labels.at(fid);

    return plc;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void cli2PLC(const char          * filename,
             const double          hatch_thickness,
             std::vector<double> & coords,
             std::vector<uint>   & tris,
             std::vector<int>    & labels)
{
    obj = SlicedObj(filename, hatch_thickness);
    if(obj.size()<2) return;

    initialize();
    edge_wise_intersections();

    mesh_vertical(tris, labels);
    mesh_horizontal(tris, labels);

    for(V_data p : v_list)
    {
        coords.push_back(p.pos.x());
        coords.push_back(p.pos.y());
        coords.push_back(p.pos.z());
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void initialize()
{
    v_list.clear();
    e_list.clear();
    e_list.resize(obj.size());

    for(uint sid=0; sid<obj.size(); ++sid) // for each slice
    {
        vector<vec3d> v;
        vector<uint>  e;
        obj.slice(sid).get_segments(v,e);

        int base_addr = v_list.size();

        for(vec3d p : v)
        {
            V_data vd;
            vd.pos = p;
            vd.pos.z() = obj.slice(sid).z_coord; // sanity check
            v_list.push_back(vd);
        }

        for(uint eid=0; eid<e.size()/2; ++eid)
        {
            E_data ed;
            ed.endpoints[0] = base_addr + e[2*eid  ];
            ed.endpoints[1] = base_addr + e[2*eid+1];
            ed.slice_id = sid;
            ed.seg_id   = eid;
            ed.z_coord  = obj.slice(sid).z_coord;
            e_list[sid].push_back(ed);
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void edge_wise_intersections()
{
    for(uint sid=0; sid<obj.size()-1;        ++sid) // for each slice
    for(uint ei =0; ei <e_list[sid].size();   ++ei) // for each segment below
    for(uint ej =0; ej <e_list[sid+1].size(); ++ej) // for each segment above
    {
        process_edge_pair(v_list, e_list[sid][ei], e_list[sid+1][ej]);
    }

    lift_unlifted_edge_extrema();
    order_split_points();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void lift_unlifted_edge_extrema()
{
    for(uint sid=0; sid<obj.size()-1;       ++sid) // for each slice
    for(uint eid=0; eid<e_list[sid].size(); ++eid) // for each segment
    for(int    ext=0; ext<2;                  ++ext) // for each segment extrema
    {
        E_data & e   = e_list[sid][eid];
        int      vid = e.endpoints[ext];
        if(v_list[vid].lifted_image == -1)
        {
            v_list[vid].lifted_image = v_list.size(); // fresh_id
            V_data vd;
            vd.pos      = v_list[vid].pos;
            vd.pos.z()  = obj.slice(sid+1).z_coord;
            v_list.push_back(vd);
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void order_split_points()
{
    for(uint sid=0; sid<obj.size()-1;       ++sid) // for each slice
    for(uint eid=0; eid<e_list[sid].size(); ++eid) // for each segment
    {
        E_data & e = e_list[sid][eid];
        int vid_A  = e.endpoints[0];
        int vid_B  = e.endpoints[1];
        int vid_C  = v_list[vid_A].lifted_image;
        int vid_D  = v_list[vid_B].lifted_image;

        assert(vid_A != -1); assert(vid_B != -1);
        assert(vid_C != -1); assert(vid_D != -1);

        e.ordered_bot_splits = order_split_points(vid_A, vid_B, e.bot_splits);
        e.ordered_top_splits = order_split_points(vid_C, vid_D, e.top_splits);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

std::vector<int> order_split_points(const int vid_beg, const int vid_end, const std::set<int> & splits)
{
    vector<int> ordered_splits;
    if (splits.empty()) return ordered_splits;

    vec3d A = v_list.at(vid_beg).pos;
    vec3d B = v_list.at(vid_end).pos;
    vec3d u = B-A;
    assert(u.length() > 0);

    vector<pair<double,int>> dot_list;
    for(int vid : splits)
    {
        vec3d  P   = v_list.at(vid).pos;
        vec3d  v   = P - A;
        double dot = u.dot(v);
        if (dot < 0)
        {
            //cout << endl;
            //cout << vid_beg << "\t" << vid_end << endl;
            //cout << A << endl;
            //cout << B << endl;
            //cout << P << endl;
            //cout << "AB " << (A-B).length() << endl;
            //cout << "PA " << (P-A).length() << endl;
            //cout << "PB " << (P-B).length() << endl;
            //cout << "uv"  << u.dot(v) << endl;
            //cout << endl;
            //assert(dot < 0)
        }
        else
        if (u.dot(v) >= u.dot(u))
        {
            //cout << endl;
            //cout << vid_beg << "\t" << vid_end << endl;
            //cout << A << endl;
            //cout << B << endl;
            //cout << P << endl;
            //cout << "AB " << (A-B).length() << endl;
            //cout << "PA " << (P-A).length() << endl;
            //cout << "PB " << (P-B).length() << endl;
            //cout << "uv - uu " << u.dot(v) << "\t" << u.dot(u) << endl;
            //cout << endl;
            //assert(u.dot(v) < u.dot(u));
        }
        else
        dot_list.push_back(make_pair(dot, vid));
    }

    sort(dot_list.begin(), dot_list.end());

    for(pair<double,int> p : dot_list)
    {
        ordered_splits.push_back(p.second);
    }

    return ordered_splits;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void mesh_vertical(std::vector<uint> & tris, std::vector<int> & labels)
{
    for(uint sid=0; sid<obj.size()-1;       ++sid) // for each slice
    for(uint eid=0; eid<e_list[sid].size(); ++eid) // for each segment
    {
        E_data & e = e_list[sid][eid];
        int vids[4] =
        {
            e.endpoints[0],
            e.endpoints[1],
            v_list[e.endpoints[0]].lifted_image,
            v_list[e.endpoints[1]].lifted_image
        };
        assert(vids[0] != -1); assert(vids[1] != -1);
        assert(vids[2] != -1); assert(vids[3] != -1);

        int n_new_tris = triangulate_quad(vids, e.ordered_bot_splits, e.ordered_top_splits, tris);

        for(int i=0; i<n_new_tris; ++i) labels.push_back(SRF_FACE_VERT);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void mesh_horizontal(std::vector<uint> & tris, std::vector<int> & labels)
{    
    for(uint sid=0; sid<obj.size(); ++sid) // for each slice
    {
        std::vector<uint> segs;
        std::set<uint>    unique_slice_verts;

        for(uint eid=0; eid<e_list[sid].size(); ++eid) // for each segment
        {
            E_data & e = e_list[sid][eid];
            int  vid_A = e.endpoints[0]; assert(vid_A != -1);
            int  vid_B = e.endpoints[1]; assert(vid_B != -1);

            segs.push_back(vid_A);
            unique_slice_verts.insert(vid_A);
            for(int vid : e.ordered_bot_splits)
            {
                segs.push_back(vid);
                segs.push_back(vid);
                unique_slice_verts.insert(vid);
            }
            segs.push_back(vid_B);
            unique_slice_verts.insert(vid_B);
        }

        if (sid > 0)
        {
            for(uint eid=0; eid<e_list[sid-1].size(); ++eid) // for each segment
            {
                E_data & e = e_list[sid-1][eid];
                int  vid_A = v_list[e.endpoints[0]].lifted_image; assert(vid_A != -1);
                int  vid_B = v_list[e.endpoints[1]].lifted_image; assert(vid_B != -1);

                segs.push_back(vid_A);
                unique_slice_verts.insert(vid_A);
                for(int vid : e.ordered_top_splits)
                {
                    segs.push_back(vid);
                    segs.push_back(vid);
                    unique_slice_verts.insert(vid);
                }
                segs.push_back(vid_B);
                unique_slice_verts.insert(vid_B);
            }
        }

        std::vector<double> coords_in;
        std::vector<uint>   verts;
        std::map<uint,uint> v_map;
        for(int vid : unique_slice_verts)
        {
            verts.push_back(vid);
            v_map[vid] = v_map.size();

            coords_in.push_back(v_list[vid].pos.x());
            coords_in.push_back(v_list[vid].pos.y());
        }

        std::vector<uint> segs_in;
        for(int vid : segs)
        {
            segs_in.push_back(v_map.at(vid));
        }

        std::vector<double> holes_in, coords_out;
        std::vector<uint> tris_out;
        triangle_wrap(coords_in, segs_in, holes_in, obj.slice(sid).z_coord, "Q", coords_out, tris_out);

        Trimesh<> m_tmp;
        triangle_wrap(coords_in, segs_in, holes_in, obj.slice(sid).z_coord, "Q", m_tmp);
        m_tmp.save("./debug.off");

        //if (coords_in.size()/2 != coords_out.size()/3)
        //{
        //    std::cout << "coords in: " << coords_in.size()/2 << "\tcoords_out: " << coords_out.size()/3 << std::endl;
        //    //Trimesh m(coords_out, tris_out);
        //    //m.save("/Users/cino/Desktop/test.obj");
        //    //assert(coords_in.size()/2 == coords_out.size()/3);
        //}
        for(uint tid=0; tid<tris_out.size()/3; ++tid)
        {
            int v0 = tris_out.at(3*tid+0);
            int v1 = tris_out.at(3*tid+1);
            int v2 = tris_out.at(3*tid+2);

            int nverts = coords_in.size()/2;
            if (v0 >= nverts || v1 >= nverts || v2 >= nverts)
            {
                std::cout << "triangle " << tid << "(" << v0 << "," << v1 << "," << v2 << ") as it contains a newly generated vertex! (N_IN_VERTS " << nverts << ")" << std::endl;
                continue;
            }

            int vid_A = verts.at(v0);
            int vid_B = verts.at(v1);
            int vid_C = verts.at(v2);

            vec3d p = (v_list.at(vid_A).pos + v_list.at(vid_B).pos + v_list.at(vid_C).pos) / 3.0;

            bool belongs_to_curr = obj.slice(sid).contains(p.x(),p.y());
            bool belongs_to_prev = (sid > 0 && obj.slice(sid-1).contains(p.x(),p.y()));

            if (belongs_to_curr || belongs_to_prev)
            {
                tris.push_back(vid_A);
                tris.push_back(vid_B);
                tris.push_back(vid_C);

                if ( belongs_to_curr && !belongs_to_prev)
                {
                    labels.push_back(SRF_FACE_DOWN);
                    std::swap(tris.at(tris.size()-1),tris.at(tris.size()-2)); // flip triangle orientation
                }
                else
                if (!belongs_to_curr &&  belongs_to_prev) labels.push_back(SRF_FACE_UP);   else
                if ( belongs_to_curr &&  belongs_to_prev && (int)sid < obj.n_layers()-1) labels.push_back(INTERNAL_FACE);
                else
                {
                    labels.push_back(SRF_FACE_UP);
                }
            }
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


cinolib::Tetmesh<> PLC2tets(cinolib::Trimesh<> & PLC)
{
    if(PLC.num_verts()==0) return cinolib::Tetmesh<>();
    std::vector<double> coords_out;
    std::vector<uint>   tets_out, edges_in; // empty
    tetgen_wrap(serialized_xyz_from_vec3d(PLC.vector_verts()), serialized_vids_from_polys(PLC.vector_polys()), edges_in, "q", coords_out, tets_out);
    return cinolib::Tetmesh<>(coords_out, tets_out);
}

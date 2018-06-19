#include "tiny_chart_helper.h"
#include "common.h"
#include <cinolib/vector_serialization.h>

TinyChartHelper::TinyChartHelper(const cinolib::Trimesh<>  & plc,
                                 const std::vector<uint> & chart)
: plc(plc)
, chart(chart)
{
    bake_chains();
    merge_chains();
    push_chart_inside_PLC();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void TinyChartHelper::bake_chains()
{
    std::set<uint> inner_edges; // (Non Manifold side)
    std::set<uint> inner_verts; // (Non Manifold side)
    std::set<uint> outer_edges; // (Manifold side)
    std::set<uint> outer_verts; // (Manifold side)

    // 1) classify edges
    //
    for(uint fid : chart)
    for(uint eid : plc.adj_p2e(fid))
    {
        if (!plc.edge_is_manifold(eid))
        {
            inner_edges.insert(eid);
        }
        else
        {
            uint fid0 = plc.adj_e2p(eid).front();
            uint fid1 = plc.adj_e2p(eid).back();
            if (plc.poly_data(fid0).label != plc.poly_data(fid1).label) outer_edges.insert(eid);
        }
    }
    //std::cout << e_side_A.size() << " side A edges and " << e_side_B.size() << " side B edges found" << std::endl;

    // 2) classify vertices
    //
    std::set<uint> endpoints;
    for(uint fid : chart)
    for(uint off=0; off<3; ++off)
    {
        uint  vid      = plc.poly_vert_id(fid, off);
        bool on_side_A = false;
        bool on_side_B = false;
        for(uint eid : plc.adj_v2e(vid))
        {
            if (cinolib::CONTAINS(inner_edges, eid)) on_side_A = true;
            if (cinolib::CONTAINS(outer_edges, eid)) on_side_B = true;
        }
        if (on_side_A && on_side_B) endpoints.insert(vid); else
        if (on_side_A) inner_verts.insert(vid);               else
        if (on_side_B) outer_verts.insert(vid);
    }
    assert(endpoints.size()==2);

    beg = *(  endpoints.begin());
    end = *(++endpoints.begin());

    sort_chain(inner_edges, inner_v_chain, inner_e_chain);
    sort_chain(outer_edges, outer_v_chain, outer_e_chain);
    //std::cout << "chain A #elements: " << inner_v_chain.size() << std::endl;
    //std::cout << "chain B #elements: " << outer_v_chain.size() << std::endl;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void TinyChartHelper::merge_chains()
{
    cinolib::Curve inner_chain;
    cinolib::Curve outer_chain;
    for(uint vid : inner_v_chain) inner_chain.append_sample(plc.vert(vid));
    for(uint vid : outer_v_chain) outer_chain.append_sample(plc.vert(vid));

    std::vector<std::pair<float,uint>> sorted_samples;
    for(uint i=0; i<inner_chain.size(); ++i)
    {
        cinolib::Curve::Sample & s = inner_chain.samples().at(i);
        sorted_samples.push_back(std::make_pair(s.t, i));
    }
    uint offset = inner_chain.size();         // references for the outer chain will be (offset + index)
    for(uint i=1; i<outer_chain.size()-1; ++i) // first and last coincide with inner_chain, so I skip them
    {
        cinolib::Curve::Sample & s = outer_chain.samples().at(i);
        sorted_samples.push_back(std::make_pair(s.t, offset+i));
    }
    std::sort(sorted_samples.begin(), sorted_samples.end());

    for(auto obj : sorted_samples)
    {
        if (obj.second < offset) // inner chain
        {
            data d;
            uint index  = obj.second;
            d.is_inner  = true;
            d.vid       = inner_v_chain.at(index);
            unified_chain_data.push_back(d);
            unified_chain.append_sample(outer_chain.sample_curve_at(obj.first)); // NOTE: I am sampling the outer curve here too!!
        }
        else // outer chain
        {
            data d;
            uint index   = obj.second - offset;
            d.is_inner  = false;
            d.vid       = outer_v_chain.at(index);
            unified_chain_data.push_back(d);
            unified_chain.append_sample(outer_chain.sample_curve_at(obj.first));
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void TinyChartHelper::sort_chain(const std::set<uint>    & edges,
                                       std::vector<uint> & v_chain,
                                       std::vector<uint> & e_chain)
{
    assert(e_chain.empty());
    assert(v_chain.empty());
    v_chain.push_back(beg);
    uint count = 0;
    while (v_chain.back() != end)
    {
        for(uint eid : edges)
        {
            if (plc.edge_contains_vert(eid, v_chain.back()) && (e_chain.empty() || e_chain.back() != eid))
            {
                v_chain.push_back(plc.vert_opposite_to(eid, v_chain.back()));
                e_chain.push_back(eid);
            }
        }
        assert(++count < edges.size()*2); // fail if infinite loop
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

cinolib::Trimesh<> TinyChartHelper::push_chart_inside_PLC()
{
    std::vector<double> coords = serialized_xyz_from_vec3d(plc.vector_verts());
    std::vector<uint>   polys  = cinolib::serialized_vids_from_polys(plc.vector_polys());
    std::set<uint>      updated_tris;
    std::vector<uint>   tris_edits;

    unified_chain_data.front().new_vid = unified_chain_data.front().vid;
    unified_chain_data.back().new_vid  = unified_chain_data.back().vid;
    for(uint i=1; i<unified_chain_data.size()-1; ++i)
    {
        if (unified_chain_data.at(i).is_inner)
        {
            unified_chain_data.at(i).new_vid = coords.size()/3;
            cinolib::vec3d p = unified_chain.samples().at(i).pos;
            coords.push_back(p.x());
            coords.push_back(p.y());
            coords.push_back(p.z());
        }
        else unified_chain_data.at(i).new_vid = unified_chain_data.at(i).vid;
    }

    auto inner_e_ptr = inner_e_chain.begin();
    auto outer_e_ptr = outer_e_chain.begin();

    for(uint i=1; i<unified_chain.size(); ++i)
    {
        // bake tris on inner side
        for(uint fid : plc.adj_e2p(*inner_e_ptr))
        {
            if (plc.poly_data(fid).label != SRF_FACE_VERT) continue;
            updated_tris.insert(fid);
            tris_edits.push_back(fid);
            tris_edits.push_back(unified_chain_data.at( i ).new_vid);
            tris_edits.push_back(unified_chain_data.at(i-1).new_vid);
            tris_edits.push_back(plc.vert_opposite_to(fid, plc.edge_vert_id(*inner_e_ptr,0), plc.edge_vert_id(*inner_e_ptr,1)));
        }
        if (unified_chain_data.at(i).is_inner) ++inner_e_ptr;

        // bake tris on outer side
        for(uint fid : plc.adj_e2p(*outer_e_ptr))
        {
            if (plc.poly_data(fid).label != SRF_FACE_VERT) continue;
            updated_tris.insert(fid);
            tris_edits.push_back(fid);
            tris_edits.push_back(unified_chain_data.at( i ).new_vid);
            tris_edits.push_back(unified_chain_data.at(i-1).new_vid);
            tris_edits.push_back(plc.vert_opposite_to(fid, plc.edge_vert_id(*outer_e_ptr,0), plc.edge_vert_id(*outer_e_ptr,1)));        }
        if (!unified_chain_data.at(i).is_inner) ++outer_e_ptr;
    }

    // bake remaining triangles (vert-adjacent)
    for(uint i=1; i<unified_chain.size()-1; ++i)
    {
        int vid = unified_chain_data.at(i).vid;
        for(uint fid : plc.adj_v2p(vid))
        {
            if (plc.poly_data(fid).label != SRF_FACE_VERT) continue;
            if (cinolib::CONTAINS(updated_tris, fid))     continue;
            updated_tris.insert(fid);
            int eid = plc.edge_opposite_to(fid,vid);
            tris_edits.push_back(fid);
            tris_edits.push_back(plc.edge_vert_id(eid,0));
            tris_edits.push_back(plc.edge_vert_id(eid,1));
            tris_edits.push_back(unified_chain_data.at(i).new_vid);
        }
    }

    // REDO MESHING OF OFFENDING CHART!!! USE TRIANGLE
    //
    // split chart triangles adjacent to relocated vertices
    outer_e_ptr = outer_e_chain.begin();
    for(uint i=1; i<unified_chain.size()-1; ++i)
    {
        if (unified_chain_data.at(i).is_inner)
        {
            uint eid = *outer_e_ptr;
            int  tid = -1;
            for(uint tmp : plc.adj_e2p(eid)) if (cinolib::DOES_NOT_CONTAIN(updated_tris,tmp)) tid = tmp;
            assert(tid != -1);
            uint v0  = unified_chain_data.at(i-1).new_vid;
            uint v1  = unified_chain_data.at(i+1).new_vid;
            uint v2  = plc.vert_opposite_to(tid, plc.edge_vert_id(eid,0), plc.edge_vert_id(eid,1)); //unified_chain_data.at( i ).vid;
            uint v3  = unified_chain_data.at( i ).new_vid;
            tris_edits.push_back(tid);
            tris_edits.push_back(v0); // t0
            tris_edits.push_back(v3);
            tris_edits.push_back(v2);
            tris_edits.push_back(tid);
            tris_edits.push_back(v1); // t1
            tris_edits.push_back(v2);
            tris_edits.push_back(v3);
        }
        if (!unified_chain_data.at(i).is_inner) ++outer_e_ptr;
    }

    // set the new PLC flags
    std::vector<int> flags = plc.export_per_poly_labels();
    for(uint fid : chart) flags.at(fid) = INTERNAL_FACE;

    // apply changes to the edited PLC triangles
    std::set<int> already_edited;
    for(uint i=0; i<tris_edits.size(); i+=4)
    {
        uint fid = tris_edits.at(i);
        if (cinolib::DOES_NOT_CONTAIN(already_edited,fid))
        {
            already_edited.insert(fid);
            polys.at(3*fid+0) = tris_edits.at(i+1);
            polys.at(3*fid+1) = tris_edits.at(i+2);
            polys.at(3*fid+2) = tris_edits.at(i+3);
        }
        else
        {
            flags.push_back(plc.poly_data(fid).label); // copy the label...
            polys.push_back(tris_edits.at(i+1));
            polys.push_back(tris_edits.at(i+2));
            polys.push_back(tris_edits.at(i+3));
        }
    }

    cinolib::Trimesh<> new_PLC(coords, polys);
    for(uint fid=0; fid<new_PLC.num_polys(); ++fid)
    {
        new_PLC.poly_data(fid).label = flags.at(fid);
    }
    return new_PLC;
}


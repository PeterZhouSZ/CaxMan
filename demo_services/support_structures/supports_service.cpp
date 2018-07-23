//#include <QApplication>
//#include <cinolib/gui/qt/qt_gui_tools.h>
#include <cinolib/meshes/meshes.h>
#include <cinolib/meshes/meshes.h>
#include <cinolib/intersection.h>
#include <cinolib/geometry/vec2.h>
#include <cinolib/matrix.h>

using namespace cinolib;

typedef std::pair<vec2d,vec2d> seg2d;
typedef struct
{
    int pid;                      // triangle containing the segment
    std::vector<double> bary_beg; // barycentric coords of first  endpoint (wrt tid)
    std::vector<double> bary_end; // barycentric coords of second endpoint (wrt tid)
}
tri_seg_inters;

typedef struct
{
    double ground_area = 0.0; // cumulative area of supports segments incident on a triangle (measured on the floor)
    double lifted_area = 0.0; // cumulative area of supports segments incident on a triangle (measured on the shape)
}
TriangleCumulativeSupportData;

typedef struct
{
    double ground_area_ratio = 0.0; // ratio between cumulative support area and total overhang area (P in Jaiko's email)
    double lifted_area_ratio = 0.0; // ratio between cumulative support area and total overhang area (p in Jaiko's email)
    double vertex_area       = 0.0; // (V in Jaiko's email)
    double support_height    = 0.0; // (D in Jaiko's email)
}
VertexCumulativeSupportData;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void detect_overhangs(const Trimesh<> &, const vec3d &, const double, std::vector<uint> &);
void project_overhangs(const std::vector<uint> &, const float, Trimesh<> &, Trimesh<> &);
void draw_2d_support_lines(const Trimesh<> &, const int, std::vector<seg2d> &);
void split_2d_support_lines(const Trimesh<> &, const std::vector<seg2d> &, const double, std::vector<tri_seg_inters> &);
void extrude_supports(const std::vector<tri_seg_inters> &, const Trimesh<> &, const Trimesh<> &, const std::vector<uint> &, const double, std::vector<VertexCumulativeSupportData> &, Trimesh<> &);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void make_supports(Trimesh<> & m,
                   const double thickness,
                   std::vector<VertexCumulativeSupportData> & per_vertex_supports_data,
                   Trimesh<> & m_supports,
                   const int n_lines = 60,
                   const double angle_thresh_deg = 30.0)
{
    double floor = m.bbox().min.z() - (m.bbox().delta_z() * 0.1);

    std::vector<uint> overhang_pids;
    detect_overhangs(m, vec3d(0,0,1), angle_thresh_deg, overhang_pids);

    Trimesh<> m_overhangs; // trimesh created projecting overhang triangles to the floor
    project_overhangs(overhang_pids, floor, m, m_overhangs);

    std::vector<seg2d> supp_lines;
    draw_2d_support_lines(m, n_lines, supp_lines);

    std::vector<tri_seg_inters> supp_segments;
    split_2d_support_lines(m_overhangs, supp_lines, floor, supp_segments);

    extrude_supports(supp_segments, m, m_overhangs, overhang_pids, thickness, per_vertex_supports_data, m_supports);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void detect_overhangs(const Trimesh<>         & m,
                      const vec3d             & build_dir,
                      const double              thresh,
                            std::vector<uint> & pids)
{
    for(uint pid=0; pid<m.num_polys(); ++pid)
    {
        vec3d n = m.poly_data(pid).normal;
        if (n.angle_deg(build_dir)-90>thresh)
        {
            if(m.poly_area(pid) > 0.0001*m.mesh_area())
                pids.push_back(pid);
        }
    }
}


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void project_overhangs(const std::vector<uint> & overhang_pids,
                       const float              floor,
                             Trimesh<>        & m,
                             Trimesh<>        & m_overhangs)
{
    std::vector<vec3d> verts;
    std::vector<uint>  polys;

    for(uint pid : overhang_pids)
    {
        int base = verts.size();
        polys.push_back(base+0);
        polys.push_back(base+1);
        polys.push_back(base+2);

        for(int i=0; i<3; ++i)
        {
            vec3d p = m.poly_vert(pid,i);
            verts.push_back(vec3d(p.x(),p.y(),floor));
        }
    }
    m_overhangs = Trimesh<>(verts, polys);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void draw_2d_support_lines(const Trimesh<>          & m,
                           const int                  n_lines,
                                 std::vector<seg2d> & supp_lines)
{
    double min_y = m.bbox().min.y();
    double max_y = m.bbox().max.y();

    for(int i=0; i<n_lines; ++i)
    {
        double x = m.bbox().min.x() + double(i)*m.bbox().delta_x()/double(n_lines);
        supp_lines.push_back(std::make_pair(vec2d(x,min_y),vec2d(x,max_y)));
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void split_2d_support_lines(const Trimesh<>                   & m_overhangs,
                            const std::vector<seg2d>          & supp_lines,
                            const double                        z_floor,
                                  std::vector<tri_seg_inters> & supp_segments)
{
    for(seg2d s : supp_lines)
    {
        for(uint pid=0; pid<m_overhangs.num_polys(); ++pid)
        {
            std::set< std::vector<double>> unique_inters; // useful if the line passes through a segment

            for(int i=0; i<3; ++i)
            {
                vec3d beg = m_overhangs.poly_vert(pid, TRI_EDGES[i][0]);
                vec3d end = m_overhangs.poly_vert(pid, TRI_EDGES[i][1]);

                std::vector<vec2d> res;
                if (segment2D_intersection(s.first, s.second, vec2d(beg.x(), beg.y()), vec2d(end.x(), end.y()), res))
                {
                    for(vec2d p : res)
                    {
                        std::vector<double> bary;
                        bool check = triangle_barycentric_coords(m_overhangs.poly_vert(pid,0),
                                                                 m_overhangs.poly_vert(pid,1),
                                                                 m_overhangs.poly_vert(pid,2),
                                                                 vec3d(p.x(), p.y(), z_floor),
                                                                 bary);
                        assert(check);
                        unique_inters.insert(bary);
                    }
                }
            }

            if (unique_inters.size() == 2)
            {
                tri_seg_inters tmp;
                tmp.pid = pid;
                tmp.bary_beg = *(unique_inters.begin());
                tmp.bary_end = *(--(unique_inters.end()));
                supp_segments.push_back(tmp);
            }
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void extrude_supports(const std::vector<tri_seg_inters>        & supp_segments,
                      const Trimesh<>                          & m,
                      const Trimesh<>                          & m_overhangs,
                      const std::vector<uint>                  & overhang_tids,
                      const double                               thickness,
                      std::vector<VertexCumulativeSupportData> & per_vertex_supports_data,
                      Trimesh<>                                & m_supports)
{
    std::vector<double> coords;
    std::vector<uint>    tris;

    std::vector<TriangleCumulativeSupportData> per_face_supports_data(m.num_polys());
    double projected_overhang_area = 0.0; // S in Jaiko's email

    for(tri_seg_inters obj : supp_segments)
    {
        int og_tid = overhang_tids.at(obj.pid);

        vec3d A = m_overhangs.poly_sample_at(obj.pid, obj.bary_beg);
        vec3d B = m_overhangs.poly_sample_at(obj.pid, obj.bary_end);
        vec3d C = m.poly_sample_at(og_tid, obj.bary_beg);
        vec3d D = m.poly_sample_at(og_tid, obj.bary_end);

        // discard small supports (I am assuming 0.1 support thickening at both sides!)
        if(A.dist(B) < 0.4) continue;

        // detach the upper profile of the support from the mesh
        C.z() -= 0.01;
        D.z() -= 0.01;

        int base = coords.size()/3;
        tris.push_back(base);
        tris.push_back(base + 1);
        tris.push_back(base + 2);
        tris.push_back(base + 1);
        tris.push_back(base + 3);
        tris.push_back(base + 2);

        coords.push_back(A.x()); coords.push_back(A.y()); coords.push_back(A.z());
        coords.push_back(B.x()); coords.push_back(B.y()); coords.push_back(B.z());
        coords.push_back(C.x()); coords.push_back(C.y()); coords.push_back(C.z());
        coords.push_back(D.x()); coords.push_back(D.y()); coords.push_back(D.z());

        projected_overhang_area += m_overhangs.poly_area(obj.pid);

        per_face_supports_data.at(og_tid).ground_area += thickness * A.dist(B);
        per_face_supports_data.at(og_tid).lifted_area += thickness * C.dist(D) * 0.3;
        // I am assuming to shape the lifted profile with tiny dents, which
        // cover 1/3 of the lifted segment. This is why I am doing *0.3...
    }

    m_supports = Trimesh<>(coords, tris);

    per_vertex_supports_data.resize(m.num_verts());

    for(uint vid=0; vid<m.num_verts(); ++vid)
    {
        per_vertex_supports_data.at(vid).vertex_area    = m.vert_mass(vid);
        per_vertex_supports_data.at(vid).support_height = m.vert(vid).z();

        double per_vert_ground_area = 0.0;
        double per_vert_lifted_area = 0.0;
        for(uint tid : m.adj_v2p(vid))
        {
            per_vert_ground_area += per_face_supports_data.at(tid).ground_area;
            per_vert_lifted_area += per_face_supports_data.at(tid).lifted_area;
        }

        per_vertex_supports_data.at(vid).ground_area_ratio = per_vert_ground_area / projected_overhang_area;
        per_vertex_supports_data.at(vid).lifted_area_ratio = per_vert_lifted_area / projected_overhang_area;
    }

}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    if(argc<4)
    {
        std::cout << "                                                                        " << std::endl;
        std::cout << "expected usage: supports_service mesh outmesh density [thresh=30]               " << std::endl;
        std::cout << "                                                                        " << std::endl;
        std::cout << "    mesh    : a triangle mesh, either in OFF or OBJ format              " << std::endl;
        std::cout << "    outmesh : a triangle mesh, either in OFF or OBJ format              " << std::endl;
        std::cout << "    density : defined in [0,1] - 0:sparse supports, 1:dense supports    " << std::endl;
        std::cout << "    thresh  : overhang threshold (default 30 degrees)                   " << std::endl;
        std::cout << "                                                                        " << std::endl;
        exit(-1);
    }

    //QApplication a(argc, argv);

    Trimesh<> m_obj(argv[1]);
    Trimesh<> m_supports;

    std::string outmesh = argv[2];

    double angle_thresh_deg = (argc>4)?atof(argv[4]):30.0;
    int    density = 20 + 100.0 * atof(argv[3]);

    std::vector<VertexCumulativeSupportData> data;
    make_supports(m_obj, 0.1, data, m_supports, density, angle_thresh_deg);

    m_obj += m_supports;
    m_supports.save("./output(supports_only).off");
    m_obj.save(outmesh.c_str());

    //m_obj.show_mesh_flat();
    //m_obj.show_wireframe(true);
    //m_obj.show_marked_edge(false);
    //m_supports.poly_set_color(Color::PASTEL_YELLOW());
    //m_supports.show_wireframe(false);
    //m_supports.show_marked_edge(false);

    //GLcanvas gui;
    //gui.push_obj(&m_obj);
    //gui.push_obj(&m_supports);
    //gui.show();

    //return a.exec();

    return 0;
}

//#include <QApplication>
//#include <cinolib/gui/qt/qt_gui_tools.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/meshes/meshes.h>
#include <cinolib/sphere_coverage.h>

using namespace cinolib;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

double get_overhangs(const Trimesh<>         & m,
                     const vec3d             & build_dir,
                     const double              angle_thresh_deg,
                           std::vector<uint> & overhang_polys)
{
    overhang_polys.clear();

    double sup_vol = 0.0;
    double tot_vol = 0.0;
    double floor   = m.bbox().min.z();

    for(uint pid=0; pid<m.num_polys(); ++pid)
    {
        vec3d  n      = m.poly_data(pid).normal;
        double angle  = acos(n.dot(build_dir)) * 180.0/M_PI;
        double height = m.poly_centroid(pid).z() - floor; assert(height>=0);
        double vol    = m.poly_mass(pid) * height;
        tot_vol      += vol;

        if (angle-90.0 >= angle_thresh_deg)
        {
            overhang_polys.push_back(pid);
            sup_vol += vol;
        }
    }
    //std::cout << "Tot Volume: " << tot_vol << std::endl;
    //std::cout << "Sup Volume: " << sup_vol << " (" << 100.0*sup_vol/tot_vol << "%)\n" << std::endl;
    return sup_vol/tot_vol;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void define_rotation(const vec3d & build_dir, vec3d & axis, double & angle)
{
    vec3d z = vec3d(0,0,1);
    axis    = build_dir.cross(z); axis.normalize();
    angle   = acos(build_dir.dot(z));
    assert(!std::isnan(angle));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

bool rotated_bbox_exceeds_chamber_size(const Trimesh<> & m, const vec3d & build_dir)
{
    Trimesh<> tmp_m = m;
    vec3d     axis;
    double    angle;
    define_rotation(build_dir, axis, angle);
    tmp_m.rotate(axis, angle);
    //if (tmp_m.bbox().delta_x() > m.global_annotations().printer.chamber_dimension[0])  return true;
    //if (tmp_m.bbox().delta_y() > m.global_annotations().printer.chamber_dimension[1])  return true;
    //if (tmp_m.bbox().delta_z() > m.global_annotations().printer.chamber_dimension[2]) return true;
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

double rotated_bbox_delta_z(const Trimesh<> & m, const vec3d & build_dir)
{
    Trimesh<> tmp_m = m;
    vec3d     axis;
    double    angle;
    define_rotation(build_dir, axis, angle);
    tmp_m.rotate(axis, angle);
    return tmp_m.bbox().delta_z();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

double cusp_height_error(const Trimesh<>& m, const vec3d & build_dir)
{
    Trimesh<> tmp_m = m;
    vec3d     axis;
    double    angle;
    define_rotation(build_dir, axis, angle);
    tmp_m.rotate(axis, angle);

    double M   = 0.0;
    double ch  = 0.0;
    for(uint pid=0; pid<tmp_m.num_polys(); ++pid)
    {
        vec3d  n = tmp_m.poly_data(pid).normal;
        double m = tmp_m.poly_mass(pid);
        ch += m * (n.dot(build_dir));
        M += m;
    }
    ch/=M;
    return ch;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

/* Choose the best orientation according to three metrics (or any combination of them).
 * Metrics are:
 *
 *  - Maximal Surface Quality (i.e. minimize cusp height error)
 *  - Minimal print time (i.e., minimize number of slices)
 *  - Minimal Support Structures (i.e., minimize overhangs)
 *
 * The influence of these metrics is defined by three scalars (wgt_srf_quality, wgt_print_time, wgt_supports).
 * This function minimizes a functional defined as the weighted sum of these three components. Weigths must
 * sum up to 1.
*/
bool orient(Trimesh<>    & m,
            const double   wgt_srf_quality,
            const double   wgt_print_time,
            const double   wgt_supports,
            const double   angle_thresh_deg,
            const int      n_dirs)
{
    std::vector<vec3d> dir_pool;
    sphere_coverage(n_dirs, dir_pool);

    double            best_obj = FLT_MAX;
    vec3d             best_dir;
    std::vector<uint> best_supports;

    for(vec3d build_dir : dir_pool)
    {
        build_dir.normalize();

        std::vector<uint> overhangs;
        double sup_area    = get_overhangs(m, build_dir, angle_thresh_deg, overhangs); // \in [0,1]
        double cusp_height = cusp_height_error(m, build_dir);                          // \in [0,1]
        double height      = rotated_bbox_delta_z(m, build_dir);                       // NOT \in [0,1]!!!!!!!

        double obj = wgt_srf_quality * cusp_height +
                     wgt_print_time  * height      +
                     wgt_supports    * sup_area;

        if(obj < best_obj)
        {
            best_obj      = obj;
            best_dir      = build_dir;
            best_supports = overhangs;
        }
    }

    if (best_obj == FLT_MAX)
    {
        std::cout << "The part cannot be oriented so as to fit the printing chamber!" << std::endl;
        return false;
    }
    else
    {
        std::cout << "OPTIMUM REACHED - Energy: " << best_obj << std::endl;
        std::vector<uint> overhangs;
        get_overhangs(m, best_dir, angle_thresh_deg, overhangs);
        for(int pid : best_supports) m.poly_data(pid).color = Color::RED();
    }

    vec3d  axis;
    double angle;
    define_rotation(best_dir, axis, angle);
    m.rotate(axis, angle);
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char *argv[])
{
    if(argc<5)
    {
        std::cout << "                                                                        " << std::endl;
        std::cout << "expected usage: orientatio_service inmesh outmesh wq wt ws [thresh=30] [ndirs=500]" << std::endl;
        std::cout << "                                                                        " << std::endl;
        std::cout << "    inmesh  : a triangle mesh, either in OFF or OBJ format                " << std::endl;
        std::cout << "    outmesh  : a triangle mesh, either in OFF or OBJ format                " << std::endl;
        std::cout << "    wq    : relevance of surface quality in final orientation           " << std::endl;
        std::cout << "    wt    : relevance of print time in final orientation                " << std::endl;
        std::cout << "    ws    : relevance of supports' amount in final orientation          " << std::endl;
        std::cout << "    thresh: overhang threshold (default 30 degrees)                     " << std::endl;
        std::cout << "    ndirs : number of candidate build directions to test (default 500) " << std::endl;
        std::cout << "                                                                        " << std::endl;
        std::cout << "    NOTE: wq, wt and ws should sum up to 1!                             " << std::endl;
        std::cout << "                                                                        " << std::endl;
        exit(-1);
    }

    //QApplication a(argc, argv);

    Trimesh<> m(argv[1]);
    //m.show_mesh_flat();

    string out_filename = argv[2];

    double wgt_srf_quality  = atof(argv[2]);
    double wgt_print_time   = atof(argv[3]);
    double wgt_supports     = atof(argv[4]);
    double angle_thresh_deg = (argc>5)?atof(argv[5]):30.0;
    int    n_dirs           = (argc>6)?atoi(argv[6]):500;

    orient(m, wgt_srf_quality, wgt_print_time, wgt_supports, angle_thresh_deg, n_dirs);

    m.save(out_filename.c_str());

    //m.updateGL();
    //GLcanvas gui;
    //gui.push_obj(&m);
    //gui.show();
    //return a.exec();

    return 0;
}

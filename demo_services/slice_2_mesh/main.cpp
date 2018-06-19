#include <iostream>
#include <vector>

#include "filter_plc.h"
#include "cli_to_vol.h"
#include "common.h"
#include "segmentation.h"

#include <cinolib/string_utilities.h>
#include <cinolib/io/read_CLI.h>
#include <cinolib/drawable_sliced_object.h>

// default parameters
double      hatch_thickness = 0.0;    // if 0 hatches (i.e. supports) won't be considered, if > 0 will be thickened and added to the mesh
bool        bake_vp         = false;  // if true a surface mesh describing the boundary of virtual prototype will be produced
bool        vp_only         = false;  // do only the virtual prototype (no volumetric mesh will be produced)
bool        verbose         = false;  // enable the verbose mode
std::string file_in;

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void set_parameters(int argc, char *argv[])
{
    file_in = std::string(argv[1]);

    for(int i=2; i<argc; ++i)
    {
        if (strcmp(argv[i], "-verbose") == 0)
        {
            verbose = true;
            std::cout << "info: enable verbose mode" << std::endl;
        }
        if (strcmp(argv[i], "-vp") == 0)
        {
            bake_vp = true;
            std::cout << "info: enable the generation of the virtual prototype" << std::endl;
        }
        if (strcmp(argv[i], "-vp-only") == 0)
        {
            bake_vp = true;
            vp_only = true;
            std::cout << "info: enable the generation of the virtual prototype (no volumetric mesh will be produced!)" << std::endl;
        }
        if (strcmp(argv[i], "-with-hatch") == 0 && i+1<argc)
        {
            hatch_thickness = atof(argv[++i]);
            std::cout << "info: enable hatch meshing (hatch thickness is " << hatch_thickness << ")" << std::endl;
        }

    }
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "                                                      " << std::endl;
        std::cout << "expected usage: slice2mesh input.cli [hatch_thickness]" << std::endl;
        std::cout << "                                                      " << std::endl;

        //std::cout << "Flags:                                                                          " << std::endl;
        //std::cout << "  -verbose       enable verbose mode                                            " << std::endl;
        //std::cout << "  -with-hatch t  thicken hatches (i.e. supports) and include them in the output " << std::endl;
        //std::cout << "  -vp            export the Virtual Prototype                                   " << std::endl;
        //std::cout << "  -vp-only       export ONLY the Virtual Prototype\n\n                          " << std::endl;
        return -1;
    }

    //std::vector<std::vector<std::vector<vec3d>>> internal_polylines;
    //std::vector<std::vector<std::vector<vec3d>>> external_polylines;
    //std::vector<std::vector<std::vector<vec3d>>> open_polylines;
    //std::vector<std::vector<std::vector<vec3d>>> hatches;
    //read_CLI(argv[1],internal_polylines,external_polylines,open_polylines,hatches);
    //exit(0);

    //set_parameters(argc, argv);
    //Trimesh<> plc = cli2PLC(file_in.c_str(), hatch_thickness);

    Trimesh<> plc = cli2PLC(argv[1], (argc>4) ? atof(argv[4]) : 0);
    plc.save(argv[2]);

    Tetmesh<> tets = PLC2tets(plc);
    tets.save(argv[3]);

    //std::vector< std::vector<uint> > tiny_charts;
    //detect_tiny_charts(plc, 0.3, tiny_charts);

//    if (bake_vp)
//    {
//        std::string vp_filename = file_in;
//        vp_filename.resize(file_in.size()-4);
//        vp_filename.append("_vp.off");
//        std::vector<double> coords_out;
//        std::vector<uint>   tris_out, dummy;
//        filter_PLC_by_label(plc, SRF_FACE_UP|SRF_FACE_DOWN|SRF_FACE_VERT, coords_out, tris_out);
//        write_OFF(vp_filename.c_str(), coords_out, tris_out, dummy);
//    }

//    if (!vp_only)
//    {
//        std::string obj_filename = file_in;
//        obj_filename.resize(file_in.size()-4);
//        obj_filename.append("_PLC.off");
//        plc.save(obj_filename.c_str());
//        obj_filename.resize(file_in.size()-4);
//        obj_filename.append("_colored_PLC.obj");
//        plc.save(obj_filename.c_str());

//        Tetmesh<> tets = PLC2tets(plc);
//        std::string tets_filename = file_in;
//        tets_filename.resize(file_in.size()-4);
//        tets_filename.append(".vtu");
//        tets.save(tets_filename.c_str());
//    }

    return 0;
}

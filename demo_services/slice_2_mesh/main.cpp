#include <cinolib/sliced_object.h>
#include <cinolib/profiler.h>
#include "common.h"
#include "slice2plc.h"
#include "plc2tet.h"

using namespace cinolib;

// default parameters
double      hatch_thickness = 0.01;
bool        export_plc      = false;
bool        export_tetmesh  = true;
std::string tetgen_flags    = "Q";
std::string plc_filename;

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void set_parameters(int argc, char *argv[])
{
    for(int i=3; i<argc; ++i)
    {
        if(strcmp(argv[i], "-plc") == 0 && i+1<argc)
        {
            export_plc = true;
            plc_filename = std::string(argv[++i]);
            std::cout << "info: export PLC on: " << plc_filename << std::endl;
        }
        if(strcmp(argv[i], "-plc-only") == 0 && i+1<argc)
        {
            export_plc     = true;
            export_tetmesh = false;
            plc_filename = std::string(argv[++i]);
            std::cout << "info: export ONLY the PLC on: " << plc_filename << std::endl;
        }
        if(strcmp(argv[i], "-hatch") == 0 && i+1<argc)
        {
            hatch_thickness = atof(argv[++i]);
            std::cout << "info: set hatch thickness to " << hatch_thickness << std::endl;
        }
        if(strcmp(argv[i], "-tetflags") == 0 && i+1<argc)
        {
            tetgen_flags = std::string(argv[++i]);
            std::cout << "info: set tetgen flags to " << tetgen_flags << std::endl;
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        std::cout << "                                                                                       " << std::endl;
        std::cout << "expected usage: slice2mesh input.cli output.mesh [flags]                                           " << std::endl;
        std::cout << "                                                                                       " << std::endl;
        std::cout << "Flags:                                                                                 " << std::endl;
        std::cout << "  -hatch    h  thicken 1D hatches by h amount and mesh them (default h=0.01)           " << std::endl;
        std::cout << "  -tetflags f  use f flags when calling tetgen to produce the tetmesh (default f=\"Q\")" << std::endl;
        std::cout << "  -plc      s  export the PLC on a file s (it's a non manifold trimesh)                " << std::endl;
        std::cout << "  -plc-only s  export ONLY the PLC                                                     " << std::endl;
        //std::cout << "  -subsmp   f  slice subsampling. consider only one every f slices\n\n                 " << std::endl;
        return -1;
    }

    set_parameters(argc,argv);

    Profiler profiler;

    profiler.push("Create Sliced Obj");
    SlicedObj<> obj(argv[1], hatch_thickness);
    profiler.pop();

    if(obj.num_slices()<2)
    {
        std::cerr << "ERROR: less than two slices were found!" << std::endl;
        exit(0);
    }

    Trimesh<> plc;
    profiler.push("slice2plc");
    slice2plc(obj, plc);
    profiler.pop();

    if(export_plc) plc.save(plc_filename.c_str());

    Tetmesh<> m;
    if(export_tetmesh)
    {
        profiler.push("plc2mesh");
        plc2tet(plc, obj, tetgen_flags.c_str(), m);
        profiler.pop();
        m.save(argv[2]);
    }

    return 0;
}

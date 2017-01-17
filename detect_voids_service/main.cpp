#include <process_plan/voids.h>
#include <trimesh/trimesh.h>
#include <tetmesh/tetmesh.h>
#include <tetgen_wrap.h>

#include <iostream>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        caxlib::logger << "Usage: ./detect_voids input.zip output.zip" << caxlib::endl;
        return 0;
    }

    caxlib::Trimesh m(argv[1]);

    caxlib::detect_voids(m); //, angle_thresh, dirs_pool_size);

    m.save(argv[2]);

    return 0;
}

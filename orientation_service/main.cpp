#include <caxlib/process_plan/orient.h>
#include <caxlib/trimesh/trimesh.h>

#include <iostream>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        caxlib::logger << "Usage: ./orientation_service input.zip output.zip" << caxlib::endl;
        return 0;
    }

    double angle_thresh   = 30.0;
    int    dirs_pool_size = 100;

    caxlib::Trimesh m(argv[1]);
    caxlib::orient(m, angle_thresh, dirs_pool_size);

    m.save(argv[2]);

    return 0;
}

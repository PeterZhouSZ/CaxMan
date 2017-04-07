#include "caxlib/process_plan/support_structures.h"
#include <trimesh/drawable_trimesh.h>

#include <iostream>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        caxlib::logger << "Usage: ./supports_service input.zip output.zip" << caxlib::endl;
        return 0;
    }

    caxlib::DrawableTrimesh m(argv[1]);
    caxlib::create_support_structures(m);

    m.save(argv[2]);

    return 0;
}

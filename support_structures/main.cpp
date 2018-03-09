#include <caxlib/process_plan/support_structures.h>
#include <caxlib/trimesh/trimesh.h>

#include <iostream>

int main(int argc, char *argv[])
{
    if (argc != 9)
    {
        caxlib::logger << "Usage: ./supports_service input.zip density thickness top_height top_length base_disp angle_thresh output.zip" << caxlib::endl;
        return 0;
    }

    // see email from jaiko@ge.imati.cnr.it, object: "Strutture di supporto"
    // Mar 8, 2018
    //
    double density      = 0.7;  // mm
    double thickness    = 0.1;  // mm
    double top_height   = 1.2;  // mm
    double top_length   = 0.2;  // mm
    double base_disp    = 4.0;  // mm
    double angle_thresh = 45.0; // deg

    std::vector<caxlib::VertexCumulativeSupportData> per_vertex_supports_data;

    caxlib::Trimesh m(argv[1]);
    caxlib::create_support_structures(m, atof(argv[2]), per_vertex_supports_data);

    m.save(argv[3]);

    FILE *f = fopen(argv[4], "w");
    if (f)
    {
        for(uint vid=0; vid<per_vertex_supports_data.size(); ++vid)
        {
            fprintf(f, "%f %f %f %f %f %f %f\n",
                    m.vertex(vid).x(),
                    m.vertex(vid).y(),
                    m.vertex(vid).z(),
                    per_vertex_supports_data.at(vid).ground_area_ratio,
                    per_vertex_supports_data.at(vid).vertex_area,
                    per_vertex_supports_data.at(vid).support_height,
                    per_vertex_supports_data.at(vid).lifted_area_ratio);
        }

        fclose(f);
    }

    return 0;
}

#include <cinolib/meshes/meshes.h>
#include <cinolib/profiler.h>

#include "sliced_obj.h"
#include "load_cli.h"

using namespace std;

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

std::ostream & operator<<(std::ostream & in, const SlicedObj & obj)
{
    for(const auto & s : obj)
    {
        in << "slice at z = " << s.z_coord << "\n";
        for(const auto & p : s)
        {
            in << "   " << p << "\n";
        }
    }
    return in;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

SlicedObj::SlicedObj(const char * filename, const double machine_precision)
{
    load_CLI(filename, machine_precision, *this);
    append_lid();
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

const Slice & SlicedObj::slice(const int layer) const
{
    return at(layer);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

Slice & SlicedObj::slice(const int layer)
{
    return at(layer);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int SlicedObj::n_layers() const
{
    return size();
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

double SlicedObj::layer_thickness(const int l) const
{
    assert(l >= 0);
    assert(l < n_layers());

    // for the final slice we do not know the thickness (there is nothing on top of it!)
    //
    if (l == n_layers()-1) return layer_thickness(l-1);

    return (slice(l+1).z_coord - slice(l).z_coord);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

Trimesh<> SlicedObj::export_wireframe_for_debug() const
{
    vector<double> coords;
    vector<uint>   tris;

    for(uint l=0; l<size(); ++l) // for each slice
    {
        vector<vec3d> v;
        vector<uint>  e;
        slice(l).get_segments(v,e);

        for(vec3d vi : v) cout << vi << endl;

        for(uint eid=0; eid<e.size()/2; ++eid)
        {
            int v0 = e.at(2*eid+0);
            int v1 = e.at(2*eid+1);

            int base_addr = coords.size()/3;

            coords.push_back(v.at(v0).x());
            coords.push_back(v.at(v0).y());
            coords.push_back(v.at(v0).z());

            coords.push_back(v.at(v0).x());
            coords.push_back(v.at(v0).y());
            coords.push_back(v.at(v0).z());

            coords.push_back(v.at(v1).x());
            coords.push_back(v.at(v1).y());
            coords.push_back(v.at(v1).z());

            tris.push_back(base_addr + 0);
            tris.push_back(base_addr + 1);
            tris.push_back(base_addr + 2);
        }
    }

    return Trimesh<>(coords, tris);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

Trimesh<> SlicedObj::triangulate() const
{
    vector<double> coords;
    vector<uint>   tris;
    for(Slice s : *this) s.triangulate(coords, tris);
    return Trimesh<>(coords, tris);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// append a lid on top of the last slice
//
void SlicedObj::append_lid()
{
    assert(size() > 1);
    double new_z = back().z_coord + layer_thickness(n_layers() - 2);
    push_back(back());
    back().z_coord = new_z;
}

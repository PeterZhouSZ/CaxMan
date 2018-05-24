#include <cinolib/meshes/meshes.h>
#include <cinolib/triangle_wrap.h>

#include "slice.h"

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/multi_polygon.hpp>


using namespace std;


std::ostream & operator<<(std::ostream & in, const Slice & s)
{
    in << "Slice:\n";
    for(auto p : s) in << p << "\n";
    return in;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

Slice::Slice(const std::vector<Polygon2D> & poly, const double z)
{
    copy(poly.begin(), poly.end(), back_inserter(*this));
    z_coord = z;

    resolve_self_intersections();
    //remove_needles();
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//Slice::Slice(const BoostMultiPolygon & poly, const double z)
//{
//    for(auto p : poly) push_back(Polygon2D(p));
//    z_coord = z;

//    resolve_self_intersections();
//    remove_needles();
//}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void Slice::resolve_self_intersections()
{
    if (size() < 2) return;

    BoostMultiPolygon multi_poly;
    multi_poly.push_back(front().poly);
    for(uint i=1; i<size(); ++i)
    {
        if (boost::geometry::intersects(multi_poly, at(i).poly))
        {
            BoostMultiPolygon tmp;
            boost::geometry::union_(multi_poly, at(i).poly, tmp);
            multi_poly = tmp;
        }
        else multi_poly.push_back(at(i).poly);
    }

    clear();
    for(auto p : multi_poly) push_back(Polygon2D(p));
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void Slice::remove_needles()
{
    for(Polygon2D p : *this)
    {
        int n_rings = 1 + p.n_holes();
        for(int r=0; r<n_rings; ++r)
        {
            vector<vec3d> ring;
            p.get_3D_points(z_coord, ring, r);
            assert((ring.front() - ring.back()).length() > 0);

            for(uint i=0; i<ring.size(); ++i)
            {
                int j = (i+1)%ring.size();
                int k = (i>0) ? i-1 : ring.size()-1;

                vec3d curr = ring[i];
                vec3d prev = ring[j];
                vec3d next = ring[k];
                vec3d u    = prev - curr;
                vec3d v    = next - curr;
                vec3d un   = u; un.normalize();
                vec3d vn   = v; vn.normalize();

                if (fabs(un.dot(vn) - 1.0) < 1e-5)
                {
                    cout << "Remove needle from slice! " << r << " " << i << " " << j << " " << k << endl;
                    vec3d new_pos = (u.length()>v.length()) ? curr + u.dot(v)*u : u.dot(v)*v;
                    cout << curr << "\t " << new_pos << endl;
                    p.set_coords(r, i, new_pos.x(), new_pos.y());
                }
            }
        }
    }
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void Slice::get_segments(std::vector<vec3d> & points, std::vector<uint> & segs) const
{
    assert(points.empty());
    assert(segs.empty());

    for(Polygon2D p : *this)
    {
        vector<vec3d> tmp_points;
        vector<uint>  tmp_segs;
        p.get_3D_segments(z_coord, tmp_points, tmp_segs);

        int base_index = points.size();
        for(auto & vid : tmp_segs) vid += base_index;
        copy(tmp_segs.begin(),   tmp_segs.end(),   back_inserter(segs));
        copy(tmp_points.begin(), tmp_points.end(), back_inserter(points));
    }
}


//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

bool Slice::contains(const double x, const double y) const
{
    for(Polygon2D p : *this)
    {
        if (p.is_inside(x,y,true)) return true;
    }
    return false;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void Slice::triangulate(std::vector<double> & coords,
                        std::vector<uint>   & tris) const
{
    for(const Polygon2D poly : *this)
    {
        vector<double> tmp_coords;
        vector<uint>   tmp_tris;

        poly.triangulate(tmp_coords, tmp_tris, z_coord);

        int base_v_addr = coords.size()/3;
        for(auto & t : tmp_tris) t += base_v_addr;

        copy(tmp_coords.begin(), tmp_coords.end(), back_inserter(coords));
        copy(tmp_tris.begin(),   tmp_tris.end(),   back_inserter(tris));
    }
}

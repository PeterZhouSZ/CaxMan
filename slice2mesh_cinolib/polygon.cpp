#include <cinolib/meshes/meshes.h>
#include <cinolib/triangle_wrap.h>
#include <cinolib/profiler.h>

#include "polygon.h"

#include <assert.h>
#include <iostream>
#include <boost/geometry.hpp>
#include <boost/geometry/algorithms/correct.hpp>
#include <boost/geometry/algorithms/within.hpp>
#include <boost/geometry/algorithms/covered_by.hpp>
#include <boost/geometry/algorithms/intersection.hpp>
#include <boost/geometry/algorithms/difference.hpp>
#include <boost/geometry/algorithms/sym_difference.hpp>
#include <boost/geometry/algorithms/union.hpp>
#include <boost/geometry/algorithms/simplify.hpp>


using namespace std;

std::ostream & operator<<(std::ostream & in, const Polygon2D & p)
{
    in << boost::geometry::dsv(p.poly);
    return in;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

Polygon2D::Polygon2D()
{}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

Polygon2D::Polygon2D(const BoostPolygon & p) : poly(p)
{
    remove_duplicated_vertices();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

Polygon2D::Polygon2D(const std::vector<double> & ring)
{
    for(uint vid=0; vid<ring.size()/2; ++vid)
    {
        boost::geometry::append(poly, BoostPoint(ring[2*vid], ring[2*vid+1]));
    }
    boost::geometry::correct(poly);

    remove_duplicated_vertices();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// r[0] ===========> outer ring
// r[1] .. r[n-1] => inner rings
//
Polygon2D::Polygon2D(const std::vector<std::vector<double> > & rings)
{
    poly.inners().resize(rings.size()-1);

    for(uint r=0; r<rings.size(); ++r)
    {
        for(uint vid=0; vid<rings[r].size()/2; ++vid)
        {
            if (r==0)
            {
                boost::geometry::append(poly, BoostPoint(rings[r][2*vid], rings[r][2*vid+1]));
            }
            else
            {
                boost::geometry::append(poly.inners()[r-1], BoostPoint(rings[r][2*vid], rings[r][2*vid+1]));
            }
        }
    }
    boost::geometry::correct(poly);

    remove_duplicated_vertices();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

Polygon2D::Polygon2D(const std::vector<double>               & outer_ring,
                     const std::vector<std::vector<double> > & inner_rings)
{
    poly.inners().resize(inner_rings.size());

    for(uint vid=0; vid<outer_ring.size()/2; ++vid)
    {
        boost::geometry::append(poly, BoostPoint(outer_ring[2*vid], outer_ring[2*vid+1]));
    }
    for(uint h=0; h<inner_rings.size(); ++h)
    {
        for(uint vid=0; vid<inner_rings[h].size()/2; ++vid)
        {
            boost::geometry::append(poly.inners()[h], BoostPoint(inner_rings[h][2*vid], inner_rings[h][2*vid+1]));
        }
    }
    boost::geometry::correct(poly);

    remove_duplicated_vertices();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int Polygon2D::n_vertices() const
{
    return n_out_vertices() + n_hole_vertices();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int Polygon2D::n_holes() const
{
    return poly.inners().size();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int Polygon2D::n_hole_vertices() const
{
    int count = 0;
    for(const auto & hole : poly.inners()) count += std::max<int>(0, hole.size() - 1);
    return count;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int Polygon2D::n_hole_vertices(const int hid) const
{
    assert(hid >= 0);
    assert(hid < n_holes());
    return poly.inners()[hid].size() - 1;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void Polygon2D::get_coords(const int ring, const int pos, double &x, double &y) const
{
    assert(ring >= 0);
    assert(pos >= 0);

    if (ring == 0)
    {
        assert(poly.outer().size() > (uint)pos);
        x = boost::geometry::get<0>(poly.outer()[pos]);
        y = boost::geometry::get<1>(poly.outer()[pos]);
    }
    else
    {
        assert((uint)ring-1 < poly.inners().size());
        assert(poly.inners()[ring-1].size() > (uint)pos);
        x = boost::geometry::get<0>(poly.inners()[ring-1][pos]);
        y = boost::geometry::get<1>(poly.inners()[ring-1][pos]);
    }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void Polygon2D::set_coords(const int ring, const int pos, const double x, const double y)
{
    assert(ring >= 0);
    assert(pos >= 0);

    if (ring == 0)
    {
        assert(poly.outer().size() > (uint)pos);
        boost::geometry::set<0>(poly.outer()[pos], x);
        boost::geometry::set<1>(poly.outer()[pos], y);
    }
    else
    {
        assert((uint)ring-1 < poly.inners().size());
        assert(poly.inners()[ring-1].size() > (uint)pos);
        boost::geometry::set<0>(poly.inners()[ring-1][pos], x);
        boost::geometry::set<1>(poly.inners()[ring-1][pos], y);
    }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

double Polygon2D::min_x() const
{
    double x = DBL_MAX;
    for(uint vid=0; vid<poly.outer().size()-1; ++vid)
    {
        x = std::min(x, boost::geometry::get<0>(poly.outer()[vid]));
    }
    return x;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

double Polygon2D::min_y() const
{
    double y = DBL_MAX;
    for(uint vid=0; vid<poly.outer().size()-1; ++vid)
    {
        y = std::min(y, boost::geometry::get<1>(poly.outer()[vid]));
    }
    return y;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

double Polygon2D::max_x() const
{
    double x = -DBL_MAX;
    for(uint vid=0; vid<poly.outer().size()-1; ++vid)
    {
        x = std::max(x, boost::geometry::get<0>(poly.outer()[vid]));
    }
    return x;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

double Polygon2D::max_y() const
{
    double y = -DBL_MAX;
    for(uint vid=0; vid<poly.outer().size()-1; ++vid)
    {
        y = std::max(y, boost::geometry::get<1>(poly.outer()[vid]));
    }
    return y;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int Polygon2D::n_out_vertices() const
{
    return std::max<int>(0, poly.outer().size() - 1);
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void Polygon2D::triangulate(std::vector<double> & coords,
                            std::vector<uint>   & tris,
                            const double          z_coord) const
{
    assert(coords.empty());
    assert(tris.empty());

    // make an internal copy and remove duplicated vertices, if any
    // (triangle would fail make the application crash otherwise)
    //
    Polygon2D tmp = *this;
//    tmp.merge_adjacent_nearby_vertices(1e-7);
//    if (tmp.n_vertices() != n_vertices())
//    {
//        cerr << "WARNING: " << n_vertices() - tmp.n_vertices()
//             << " duplicated vertices were removed to perform triangulation" << endl;
//    }

    vector<double> coords_in;  // serialized xy coordinates
    vector<uint>   edges_in;   // serialized vertex ids
    vector<double> holes_in;   // serialized xyz coordinates (one point interior to each hole)

    assert(tmp.poly.outer().size() > 3);
    for(uint vid=0; vid<tmp.poly.outer().size()-1; ++vid)
    {
        coords_in.push_back(boost::geometry::get<0>(tmp.poly.outer()[vid]));
        coords_in.push_back(boost::geometry::get<1>(tmp.poly.outer()[vid]));

        edges_in.push_back(vid);
        edges_in.push_back((vid+1)%(tmp.poly.outer().size()-1));
    }

    vector<uint> hole_edges;

    for(const auto & hole : tmp.poly.inners())
    {
        int base_addr = coords_in.size()/2;
        int e[2];

        assert(hole.size() > 3);
        for(uint vid=0; vid<hole.size()-1; ++vid)
        {
            double x = boost::geometry::get<0>(hole[vid]);
            double y = boost::geometry::get<1>(hole[vid]);

            coords_in.push_back(x);
            coords_in.push_back(y);

            e[0] = base_addr + vid;
            e[1] = base_addr + ((vid+1)%(hole.size()-1));

            edges_in.push_back(e[0]);
            edges_in.push_back(e[1]);
        }
        assert(edges_in.size() >= 6);

        hole_edges.push_back(e[0]);
        hole_edges.push_back(e[1]);

        //BoostPolygon boost_poly_hole;
        //boost::geometry::append(boost_poly_hole, hole);
        //Polygon2D poly_hole(boost_poly_hole);
        //double x,y;
        //poly_hole.get_a_point_inside(x,y);
        //holes_in.push_back(x);
        //holes_in.push_back(y);
    }
    assert(hole_edges.size() == 2 * tmp.poly.inners().size());

    vector<double> no_holes;
    triangle_wrap(coords_in, edges_in, no_holes, z_coord, "Q", coords, tris);

    if (tmp.poly.inners().size() > 0)
    {
        for(uint hid=0; hid<hole_edges.size()/2; ++hid)
        {
            bool   found = false;
            double hx,hy;
            uint    e[2] = { hole_edges[2*hid+0] , hole_edges[2*hid+1] };
            for(uint tid=0; tid<(uint)tris.size()/3; ++tid)
            {
                uint t[3] = { tris[tid*3+0], tris[tid*3+1], tris[tid*3+2] };

                if ((e[0] == t[0] && e[1] == t[1]) ||
                    (e[0] == t[1] && e[1] == t[2]) ||
                    (e[0] == t[2] && e[1] == t[0]))
                {
                    assert(!found);
                    found = true;
                    hx = (coords[3*t[0]]   + coords[3*t[1]]   + coords[3*t[2]])   / 3.0;
                    hy = (coords[3*t[0]+1] + coords[3*t[1]+1] + coords[3*t[2]+1]) / 3.0;
                    assert(!is_inside(hx, hy, true));
                }
            }
            assert(found);
            holes_in.push_back(hx);
            holes_in.push_back(hy);
        }

        coords.clear();
        tris.clear();
        triangle_wrap(coords_in, edges_in, holes_in, z_coord, "Q", coords, tris);
    }

    // check to make sure that no Steiner points were added
    //
    if (coords_in.size()/2 != coords.size()/3)
    {
        cerr << "WARNING - Triangle: input and output vertices do not match. Discard polygon!" << endl;

        coords.clear();
        tris.clear();

        //cout.precision(17);
        //cout << coords_in.size()/2 << "\t" << coords.size()/3 << endl;
        //cout << *this << endl;
        //cout << "coords in (xy)" << endl;
        //for(auto c : coords_in) cout << c << endl;
        //cout << endl;
        //cout << "segs in" << endl;
        //for(auto c : edges_in) cout << c << endl;
        //cout << endl;
        //cout << "holes in" << endl;
        //for(auto c : holes_in) cout << c << endl;
        //cout << endl;
        //cout << "coords out (xyz)" << endl;
        //for(auto c : coords) cout << c << endl;
        //cout << endl;
        //cout << "tris out" << endl;
        //for(auto c : tris) cout << c << endl;
        //assert(coords_in.size()/2 == coords.size()/3);
    }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

vector<Polygon2D> Polygon2D::operator & (const Polygon2D & p) const
{
    assert(!poly.outer().empty());
    assert(!p.poly.outer().empty());

    vector<BoostPolygon> res;
    boost::geometry::intersection(poly, p.poly, res);

    vector<Polygon2D> out;
    for(const auto & p_res : res) out.push_back(Polygon2D(p_res));

    return out;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

vector<Polygon2D> Polygon2D::operator - (const Polygon2D & p) const
{
    assert(!poly.outer().empty());
    assert(!p.poly.outer().empty());

    vector<BoostPolygon> res;
    boost::geometry::difference(poly, p.poly, res);

    vector<Polygon2D> out;
    for(const auto & p_res : res) out.push_back(Polygon2D(p_res));

    return out;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

vector<Polygon2D> Polygon2D::operator | (const Polygon2D & p) const
{
    assert(!poly.outer().empty());
    assert(!p.poly.outer().empty());

    vector<BoostPolygon> res;
    boost::geometry::union_(poly, p.poly, res);

    vector<Polygon2D> out;
    for(const auto & p_res : res) out.push_back(Polygon2D(p_res));

    return out;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

std::vector<Polygon2D> Polygon2D::operator ^ (const Polygon2D &p) const // xor
{
    assert(!poly.outer().empty());
    assert(!p.poly.outer().empty());

    vector<BoostPolygon> res;
    boost::geometry::sym_difference(poly, p.poly, res);

    vector<Polygon2D> out;
    for(const auto & p_res : res) out.push_back(Polygon2D(p_res));

    return out;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

std::vector<Polygon2D> Polygon2D::sym_overlay(const Polygon2D & p) const // (A^B) U (A&B)
{
    vector<Polygon2D> xor_p = *this ^ p;
    vector<Polygon2D> and_p = *this & p;

    vector<Polygon2D> res;
    std::copy(xor_p.begin(), xor_p.end(), std::back_inserter(res));
    std::copy(and_p.begin(), and_p.end(), std::back_inserter(res));

    return res;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void Polygon2D::get_3D_segments(const double z_coord, std::vector<vec3d> & points,
                                                      std::vector<uint>  & segs) const
{
    assert(points.empty());
    assert(segs.empty());

    for(int ring=0; ring<1+n_holes(); ++ring)
    {
        int base_addr = points.size();
        int n_vertices = (ring == 0) ? n_out_vertices() : n_hole_vertices(ring-1);
        for(int i=0; i<n_vertices; ++i)
        {
            double x0,y0;
            get_coords(ring, i, x0, y0);
            points.push_back(vec3d(x0,y0,z_coord));
            segs.push_back(base_addr + i);
            segs.push_back(base_addr + (i+1)%n_vertices);
        }
    }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void Polygon2D::get_3D_points(const double z_coord, std::vector<vec3d> & points) const
{
    assert(points.empty());

    for(int h=0; h<n_holes()+1; ++h)
    {
        get_3D_points(z_coord, points, h);
    }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void Polygon2D::get_3D_points(const double z_coord, std::vector<vec3d> & points, int ring) const
{
    assert(ring >= 0 && ring <= n_holes());

    int n_vertices = (ring == 0) ? n_out_vertices() : n_hole_vertices(ring-1);

    for(int i=0; i<n_vertices; ++i)
    {
        double x, y;
        get_coords(ring, i, x, y);
        points.push_back(vec3d(x,y,z_coord));
    }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

std::vector<Polygon2D> Polygon2D::overlay(const Polygon2D & p) const // (A-B) U (A&B)
{
    vector<Polygon2D> min_p = *this - p;
    vector<Polygon2D> and_p = *this & p;

    vector<Polygon2D> res;
    std::copy(min_p.begin(), min_p.end(), std::back_inserter(res));
    std::copy(and_p.begin(), and_p.end(), std::back_inserter(res));

    return res;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

bool Polygon2D::is_inside(const Polygon2D & p, bool borders_count) const
{
    assert(!poly.outer().empty());

    if (borders_count) return boost::geometry::covered_by(poly, p.poly);
    return boost::geometry::within(poly, p.poly);
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

bool Polygon2D::is_inside(const double x, const double y, bool borders_count) const
{
    assert(!poly.outer().empty());

    if (borders_count) return boost::geometry::covered_by(BoostPoint(x,y), poly);
    return boost::geometry::within(BoostPoint(x,y), poly);
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

bool Polygon2D::intersects(const Polygon2D & p, bool borders_count) const
{
    assert(!poly.outer().empty());

    if (borders_count) return boost::geometry::intersects(poly, p.poly);
    return boost::geometry::overlaps(poly, p.poly);
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void Polygon2D::get_a_point_inside(double & x, double & y) const
{
    y = (max_y() + min_y()) * 0.5;

    BoostLinestring ls;
    ls.push_back(BoostPoint(min_x()-1, y));
    ls.push_back(BoostPoint(max_x()+1, y));

    vector<BoostPoint> res;
    boost::geometry::intersection(ls, poly, res);
    assert(res.size() >= 2); // this should penetrate the polygon at least twice

    vector<double> res_x;
    for(const auto & p : res) res_x.push_back(boost::geometry::get<0>(p));
    sort(res_x.begin(), res_x.end());
    x = (res_x[0] + res_x[1]) * 0.5;

    assert(is_inside(x,y,false));
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void Polygon2D::merge_adjacent_nearby_vertices(const double eps)
{
    int count = 0;

    vector< vector<double> > new_rings;
    vector< vector<vec3d> >  old_rings(1 + n_holes());

    for(int r=0; r<n_holes()+1; ++r)
    {
        get_3D_points(0, old_rings[r], r);

        vector<double> ring;

        for(uint i=0; i<old_rings[r].size(); ++i)
        {
            vec3d curr = old_rings[r][i];
            vec3d next = old_rings[r][(i+1)%old_rings[r].size()];

            if ((curr-next).length() < eps)
            {
                ++count;
                continue;
            }

            ring.push_back(curr.x());
            ring.push_back(curr.y());
        }

        if (ring.size() >= 6)
        {
            new_rings.push_back(ring);
        }
        else
        {
            cerr << "WARNING: ring oversimplified! It contains less than 3 vertices, discarded." << endl;

            if (r==0) // if it is the outer ring return an empty polygon
            {
                poly.clear();
                return;
            }
        }
    }

    Polygon2D new_poly(new_rings);
    poly = new_poly.poly;

    if (count) cout << count << " points were removed from the Polygon2D" << endl;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void Polygon2D::merge_collinear_edges(const double angle_deg)
{
    int count = 0;

    double angle_rad = angle_deg * M_PI / 180.0;

    vector< vector<double> > new_rings;
    vector< vector<vec3d> >  old_rings(1 + n_holes());

    for(int r=0; r<n_holes()+1; ++r)
    {
        get_3D_points(0, old_rings[r], r);

        vector<bool> to_remove(old_rings[r].size(), false);

        for(uint i=0; i<old_rings[r].size(); ++i)
        {
            vec3d A = old_rings[r][i];
            vec3d B = old_rings[r][(i+1)%old_rings[r].size()];
            vec3d C = old_rings[r][(i+2)%old_rings[r].size()];

            vec3d u = B - A; u.normalize();
            vec3d v = C - B; v.normalize();

            double cos = u.dot(v);

            if (cos > 0.0 && acos(cos) <= angle_rad)
            {
                ++count;
                to_remove[i+1] = true;
            }
        }

        vector<double> ring;
        for(uint i=0; i<old_rings[r].size(); ++i)
        {
            if (to_remove[i]) continue;
            ring.push_back(old_rings[r][i].x());
            ring.push_back(old_rings[r][i].y());
        }

        if (ring.size() >= 6)
        {
            new_rings.push_back(ring);
        }
        else
        {
            cerr << "WARNING: ring oversimplified! It contains less than 3 vertices, discarded." << endl;

            if (r==0) // if it is the outer ring return an empty polygon
            {
                poly.clear();
                return;
            }
        }
    }

    Polygon2D new_poly(new_rings);
    poly = new_poly.poly;

    if (count) cout << count << " colinear edge pairs were merged" << endl;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void Polygon2D::simplify(const double max_dist)
{
    // for details see:
    // http://www.boost.org/doc/libs/1_57_0/libs/geometry/doc/html/geometry/reference/algorithms/simplify/simplify_3.html
    //
    // for known bugs regarding collinear segments, see also:
    // http://boost-geometry.203548.n3.nabble.com/Simplifying-polygons-with-co-linear-points-td3415757.html
    //
    BoostPolygon res;
    boost::geometry::simplify(poly, res, max_dist);
    poly = res;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void Polygon2D::remove_duplicated_vertices()
{
    boost::geometry::unique(poly);
}

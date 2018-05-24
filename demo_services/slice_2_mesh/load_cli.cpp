#include "load_cli.h"

#include <assert.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include <boost/geometry.hpp>
#include <boost/geometry/algorithms/difference.hpp>

using namespace std;

enum
{
    INTERNAL_CW  = 0,
    EXTERNAL_CCW = 1,
    OPEN_CURVE   = 2
};


void read_polyline(string line, std::vector<double> & points)
{
    replace(line.begin(), line.end(), '$', ' ');
    replace(line.begin(), line.end(), '/', ' ');
    replace(line.begin(), line.end(), ',', ' ');

    int    n_points;
    int    dummy_int;
    string dummy_str;
    double x;
    double y;

    istringstream ss(line);
    ss >> dummy_str >> dummy_int >> dummy_int >> n_points;

    for(int i=0; i<n_points; ++i)
    {
        ss >> x >> y;
        points.push_back(x);
        points.push_back(y);
    }
}


std::vector<Polygon2D> make_open_curves(const vector< vector<double> > open_curves, const double machine_precision)
{
    // boost::buffer strategies
    //
    boost::geometry::strategy::buffer::distance_symmetric<double> distance_strategy(machine_precision);
    boost::geometry::strategy::buffer::join_miter                 join_strategy;
    boost::geometry::strategy::buffer::end_flat                   end_strategy;
    boost::geometry::strategy::buffer::point_square               circle_strategy;
    boost::geometry::strategy::buffer::side_straight              side_strategy;

    std::vector<Polygon2D> buffered_curves;
    if (open_curves.empty()) return buffered_curves;

    for(vector<double> c : open_curves)
    {
        BoostLinestring ls;
        for(uint vid=0; vid<c.size()/2; ++vid)
        {
            boost::geometry::append(ls, BoostPoint(c[2*vid],c[2*vid+1]));
        }

        std::vector<BoostPolygon> res;
        boost::geometry::buffer(ls, res, distance_strategy, side_strategy,
                                join_strategy, end_strategy, circle_strategy);
        assert(res.size()==1);
        buffered_curves.push_back(Polygon2D(res.front()));                
    }

    return buffered_curves;
}


Slice make_slice(const double                   z,
                 const double                   machine_precision,
                 const vector< vector<double> > outer_rings,
                 const vector< vector<double> > inner_holes,
                 const vector< vector<double> > open_curves)
{
    vector<Polygon2D> polys;
    vector<Polygon2D> holes;
    for(auto r : outer_rings) polys.push_back(Polygon2D(r));
    for(auto h : inner_holes) holes.push_back(Polygon2D(h));

    assert(!polys.empty());

    for(Polygon2D & r : polys)
    for(Polygon2D   h : holes)
    {
        if (h.is_inside(r, false))
        {
            vector<Polygon2D> res = r - h;
            assert(res.size() == 1);
            r = res.front();
        }
//        else assert(!h.intersects(r));
    }

    //for(Polygon2D & r : polys)
    //{
    //    r.remove_duplicated_vertices();
    //    r.merge_colinear_edges(1);
    //}

    vector<Polygon2D> supports = make_open_curves(open_curves, machine_precision);
    for(auto s : supports) polys.push_back(s);

    return Slice(polys, z);
}


void load_CLI(const char * filename, const double machine_precision, std::vector<Slice> & slices)
{
    assert(slices.empty());

    ifstream                 f(filename);
    int                      n_layers, type;
    double                   z, dummy_z;
    string                   line;
    vector< vector<double> > outer_rings;
    vector< vector<double> > holes;
    vector< vector<double> > open_curves;

    if (!f.is_open())
    {
        cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load_CLI() : couldn't open input file " << filename << endl;
        exit(-1);
    }

    while (getline(f, line, '\n'))
    {
        if (sscanf(line.c_str(), "$$LAYERS/%d", &n_layers) == 1)
        {}
        else if (sscanf(line.c_str(), "$$LAYER/%lf", &dummy_z) == 1)
        {
            if (!outer_rings.empty() || !open_curves.empty()) // append the previous layer, if any
            {
                slices.push_back(make_slice(z, machine_precision, outer_rings, holes, open_curves));
            }
            // then get ready for the new layer
            outer_rings.clear();
            holes.clear();
            open_curves.clear();
            z = dummy_z;
        }
        else if (sscanf(line.c_str(), "$$POLYLINE/%*d,%d,%*d,%*s", &type) == 1)
        {
            vector<double> points;
            read_polyline(line, points);

            if (type == EXTERNAL_CCW)
            {
                points.pop_back(); // the last point is a duplication of the first one
                outer_rings.push_back(points);
            }
            else if (type == INTERNAL_CW)
            {
                points.pop_back(); // the last point is a duplication of the first one
                holes.push_back(points);
            }
            else if (type == OPEN_CURVE)
            {
                open_curves.push_back(points);
            }
            else assert(false && "ERROR! Input non consistent with .cli format specification!");
        }
    }

    if (!outer_rings.empty() || !open_curves.empty()) // append the last layer
    {
        slices.push_back(make_slice(z, machine_precision, outer_rings, holes, open_curves));
    }

    f.close();
}

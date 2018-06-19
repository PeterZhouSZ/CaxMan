#include "edge_processing.h"

//#include "intersection.h"
#include <cinolib/intersection.h>

enum
{
    _A_ = 0,
    _B_ = 1,
    _C_ = 2,
    _D_ = 3,
};

using namespace std;

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

bool lift_bot_left(const bool is[4], const int vids[4], std::vector<V_data> & points);
bool lift_bot_right(const bool is[4], const int vids[4], std::vector<V_data> & points);

bool set_top_split (const bool is[4], const int vids[4], std::vector<V_data> & points, E_data & e_below);
bool set_bot_split (const bool is[4], const int vids[4], std::vector<V_data> & points, E_data & e_above);

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void process_edge_pair(std::vector<V_data> & points,
                       E_data              & e_below,
                       E_data              & e_above)
{
    using namespace cinolib;

    int vids[4] =
    {
        e_below.endpoints[0],
        e_below.endpoints[1],
        e_above.endpoints[0],
        e_above.endpoints[1]
    };

    vec2d A(points[vids[_A_]].pos.x(), points[vids[_A_]].pos.y());
    vec2d B(points[vids[_B_]].pos.x(), points[vids[_B_]].pos.y());
    vec2d C(points[vids[_C_]].pos.x(), points[vids[_C_]].pos.y());
    vec2d D(points[vids[_D_]].pos.x(), points[vids[_D_]].pos.y());

    vector<vec2d> res;
    segment2D_intersection(A,B,C,D,res);
    //segment_intersection_2D(A, B, C, D, res); // discard z-coord

    for(vec2d P : res)
    {
        bool is[4] = // check for similarity with segment endpoints
        {
            ((A-P).length() < 1e-10),
            ((B-P).length() < 1e-10),
            ((C-P).length() < 1e-10),
            ((D-P).length() < 1e-10)
        };

        if (is[_A_]) assert(!is[_B_]);
        if (is[_B_]) assert(!is[_A_]);
        if (is[_C_]) assert(!is[_D_]);
        if (is[_D_]) assert(!is[_C_]);

        // debug
        //if (!is[_A_] && !is[_B_] && !is[_C_] && !is[_D_])
        //{
        //    vec2d u = P - A;
        //    vec2d v = B - A;
        //    if (u.dot(v) < 0 || u.dot(v) > v.dot(v))
        //    {
        //        cout.precision(17);
        //        cout << A << "\t" << B << endl;
        //        cout << C << "\t" << D << endl;
        //        cout << P << endl;
        //        cout << "P-A dot B-A   " << u.dot(v) << endl;
        //        cout << "B-A dot B-A   " << u.dot(u) << endl;
        //        cout << "d(P,A)   " << (P-A).length() << endl;
        //        cout << "d(P,B)   " << (P-B).length() << endl;
        //        cout << "is " << is[0] << " " << is[1] << " " << is[2] << " " << is[3] << endl;
        //    }
        //    assert(u.dot(v) >= 0);
        //    assert(u.dot(v) <= v.dot(v));
        //}

        bool b0 = lift_bot_left (is, vids, points);
        bool b1 = lift_bot_right(is, vids, points);
        bool b2 = set_top_split (is, vids, points, e_below);
        bool b3 = set_bot_split (is, vids, points, e_above);

        if (b0 || b1 || b2 || b3) // if any of the routines above asks for a new vertex, add it
        {
            V_data vd;
            vd.pos.x()  = P.x();
            vd.pos.y()  = P.y();
            vd.pos.z()  = e_above.z_coord;
            points.push_back(vd);
        }
    }
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

bool lift_bot_left(const bool is[4], const int vids[4], std::vector<V_data> & points)
{
    if (points[vids[_A_]].lifted_image != -1) return false;

    if (is[_A_] && is[_C_]) points[vids[_A_]].lifted_image = vids[_C_]; else
    if (is[_A_] && is[_D_]) points[vids[_A_]].lifted_image = vids[_D_]; else
    if (is[_A_])
    {
        points[vids[_A_]].lifted_image = points.size(); // fresh_vid
        return true;
    }
    return false;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

bool lift_bot_right(const bool is[4], const int vids[4], std::vector<V_data> & points)
{
    if (points[vids[_B_]].lifted_image != -1) return false;

    if (is[_B_] && is[_C_]) points[vids[_B_]].lifted_image = vids[_C_]; else
    if (is[_B_] && is[_D_]) points[vids[_B_]].lifted_image = vids[_D_]; else
    if (is[_B_])
    {
        points[vids[_B_]].lifted_image = points.size(); // fresh_vid
        return true;
    }
    return false;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

bool set_top_split(const bool is[4], const int vids[4], std::vector<V_data> & points, E_data & e_below)
{
    if (is[_C_] && !is[_A_] && !is[_B_])
    {
        e_below.top_splits.insert(vids[_C_]);
    }
    else if (is[_D_] && !is[_A_] && !is[_B_])
    {
        e_below.top_splits.insert(vids[_D_]);
    }
    else if (!is[_A_] && !is[_B_] && !is[_C_] && !is[_D_])
    {
        e_below.top_splits.insert(points.size());  // fresh_vid
        return true;
    }
    return false;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

bool set_bot_split(const bool is[4], const int vids[4], std::vector<V_data> & points, E_data & e_above)
{
    if ( is[_A_] && !is[_C_] && !is[_D_])
    {
        assert(points[vids[_A_]].lifted_image != -1);
        e_above.bot_splits.insert(points[vids[_A_]].lifted_image);
    }
    else if ( is[_B_] && !is[_C_] && !is[_D_])
    {
        assert(points[vids[_B_]].lifted_image != -1);
        e_above.bot_splits.insert(points[vids[_B_]].lifted_image);
    }
    else if (!is[_A_] && !is[_B_] && !is[_C_] && !is[_D_])
    {
        e_above.bot_splits.insert(points.size());  // fresh_vid
        return true;
    }   
    return false;
}

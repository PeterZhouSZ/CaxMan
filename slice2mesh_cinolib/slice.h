#ifndef SLICE_H
#define SLICE_H

#include <cinolib/geometry/vec3.h>
#include "polygon.h"

class Slice : public std::vector<Polygon2D>
{
    public:

        Slice(){}
        Slice(const std::vector<Polygon2D> & poly, const double z);
//        Slice(const BoostMultiPolygon      & poly, const double z);

        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void resolve_self_intersections();
        void remove_needles();

        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void get_segments(std::vector<vec3d> & points, std::vector<uint> & segs) const;

        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        bool contains(const double x, const double y) const;

        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void triangulate(std::vector<double> & coords, std::vector<uint> & tris) const;

        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        double z_coord;
};

std::ostream & operator<<(std::ostream & in, const Slice & s);

#endif // SLICE_H

#ifndef POLYGON_H
#define POLYGON_H

#include <vector>
#include <ostream>
#include <sys/types.h>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/multi_polygon.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/linestring.hpp>

#include <cinolib/geometry/vec3.h>

typedef boost::geometry::model::d2::point_xy<double>        BoostPoint;
typedef boost::geometry::model::polygon<BoostPoint>         BoostPolygon;
typedef boost::geometry::model::multi_polygon<BoostPolygon> BoostMultiPolygon;
typedef boost::geometry::model::linestring<BoostPoint>      BoostLinestring;

using namespace cinolib;

class Polygon2D
{
    public:

        Polygon2D();

        Polygon2D(const BoostPolygon & p);

        Polygon2D(const std::vector<double> & ring);

        Polygon2D(const std::vector< std::vector<double> > & rings);

        Polygon2D(const std::vector<double>                & outer_ring,
                  const std::vector< std::vector<double> > & inner_rings);

        // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        int  n_vertices() const;
        int  n_out_vertices() const;
        int  n_holes() const;
        int  n_hole_vertices() const;
        int  n_hole_vertices(const int hid) const;

        // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        std::vector<Polygon2D> operator & (const Polygon2D & p) const; // and
        std::vector<Polygon2D> operator | (const Polygon2D & p) const; // or
        std::vector<Polygon2D> operator ^ (const Polygon2D & p) const; // xor
        std::vector<Polygon2D> operator - (const Polygon2D & p) const; // minus
        std::vector<Polygon2D> overlay    (const Polygon2D & p) const; // (A-B) U (A&B)
        std::vector<Polygon2D> sym_overlay(const Polygon2D & p) const; // (A^B) U (A&B)

        // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void   get_3D_segments(const double z_coord, std::vector<vec3d> & points,
                                                     std::vector<uint>  & segs) const;
        void   get_3D_points  (const double z_coord, std::vector<vec3d> & points)           const;
        void   get_3D_points  (const double z_coord, std::vector<vec3d> & points, int ring) const;

        // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void simplify(const double max_dist);
        void remove_duplicated_vertices();
        void merge_adjacent_nearby_vertices(const double eps);
        void merge_collinear_edges(const double angle_deg);

        // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void set_coords(const int ring, const int pos, const double x, const double y);
        void get_coords(const int ring, const int pos, double & x, double & y) const;

        // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        double min_x() const;
        double min_y() const;
        double max_x() const;
        double max_y() const;

        // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        bool   is_inside(const Polygon2D & p, bool borders_count = true) const;
        bool   is_inside(const double x, const double y, bool borders_count = true) const;
        bool   intersects(const Polygon2D & p, bool borders_count = true) const;
        void   get_a_point_inside(double & x, double & y) const;

        // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void   triangulate(std::vector<double> & coords,
                           std::vector<uint>   & tris,
                           const double          z_coord) const;

        // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        BoostPolygon poly;
};

std::ostream & operator<<(std::ostream & in, const Polygon2D & p);

#endif // POLYGON_H

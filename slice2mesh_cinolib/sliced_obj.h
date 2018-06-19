#ifndef SLICED_OBJ_H
#define SLICED_OBJ_H

#include <sys/types.h>
#include <cinolib/meshes/meshes.h>
#include "slice.h"

using namespace cinolib;

class SlicedObj : public std::vector<Slice>
{
    public:

        SlicedObj(){}
        SlicedObj(const char * filename, const double machine_precision);

        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        const Slice & slice(const int layer) const;
              Slice & slice(const int layer);

        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        int    n_layers() const;
        double layer_thickness(const int l) const;

        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        Trimesh<> export_wireframe_for_debug() const;

        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        Trimesh<> triangulate() const;

        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void append_lid();
};

std::ostream & operator<<(std::ostream & in, const SlicedObj & obj);

#endif // SLICED_OBJ_H

#ifndef TINY_CHART_HELPER_H
#define TINY_CHART_HELPER_H

#include <cinolib/meshes/meshes.h>
#include <cinolib/curve.h>

class TinyChartHelper
{
    const cinolib::Trimesh<>  & plc;
    const std::vector<uint>   & chart;

          uint                  beg, end;       // split the perimeter of the chart into two chains connecting beg and end
          std::vector<uint>     inner_e_chain;  // edge   chain (Non Manifold side)
          std::vector<uint>     inner_v_chain;  // vertex chain (Non Manifold side)
          std::vector<uint>     outer_e_chain;  // edge   chain (Manifold side)
          std::vector<uint>     outer_v_chain;  // vertex chain (Manifold side)

          typedef struct
          {
            bool is_inner = false;
            int  vid      = -1;
            int  new_vid  = -1;
          } data;

          cinolib::Curve        unified_chain;
          std::vector<data>     unified_chain_data;

    public:

        TinyChartHelper(const cinolib::Trimesh<>  & plc,
                        const std::vector<uint> & chart);

        cinolib::Trimesh<> push_chart_inside_PLC();

    private:

        void bake_chains();
        void merge_chains();
        void sort_chain(const std::set<uint> & edges, std::vector<uint> & v_chain, std::vector<uint> & e_chain);
};

#endif // TINY_CHART_HELPER_H

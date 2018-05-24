#include "cli_to_vol_debug.h"
#include <iostream>

using namespace std;

extern vector<V_data>           v_list;
extern vector< vector<E_data> > e_list;

std::ostream & operator<<(std::ostream & in, const E_data & e)
{
    in << "-----------------------------------------------" << endl;

    in << "slice: " << e.slice_id << "\tseg: " << e.seg_id << "\tz: " << e.z_coord << endl;

    int A = e.endpoints[0];
    int B = e.endpoints[1];
    int C = v_list.at(A).lifted_image;
    int D = v_list.at(B).lifted_image;

    in << "A: " << A << "\t" << v_list.at(A).pos << endl;
    in << "B: " << B << "\t" << v_list.at(B).pos << endl;
    in << "C: " << C; if (C!=-1) in << "\t" << v_list[C].pos; in << endl;
    in << "D: " << D; if (D!=-1) in << "\t" << v_list[D].pos; in << endl;

    if (!e.bot_splits.empty())
    {
        in << "Bot Splits:\n";
        for(int vid : e.bot_splits) in << vid << "\t" << v_list[vid].pos << endl;
    }
    if (!e.ordered_bot_splits.empty())
    {
        in << "Ordered Bot Splits:\n";
        for(int vid : e.bot_splits) in << vid << "\t" << v_list[vid].pos << endl;
    }
    if (!e.top_splits.empty())
    {
        in << "Top Splits:\n";
        for(int vid : e.top_splits) in << vid << "\t" << v_list[vid].pos << endl;
    }
    if (!e.ordered_top_splits.empty())
    {
        in << "Ordered Top Splits:\n";
        for(int vid : e.top_splits) in << vid << "\t" << v_list[vid].pos << endl;
    }

    in << "-----------------------------------------------" << endl;

    return in;
}

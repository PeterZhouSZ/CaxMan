#ifndef TRIANGULATE_QUAD_H
#define TRIANGULATE_QUAD_H

#include <vector>
#include <sys/types.h>

int triangulate_quad(const int                 vids[4],
                     const std::vector<int>  & bot_splits,
                     const std::vector<int>  & top_splits,
                           std::vector<uint> & tris);

#endif // TRIANGULATE_QUAD_H

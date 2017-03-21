#ifndef triangle_hpp
#define triangle_info

#include "triangle.hpp"

using namespace std;

typedef void (Triangle::*TrMemberFuncPtr)();
typedef unordered_map<string, pair<long_d*, int>> TrElemPtrMap;
typedef vector<TrMemberFuncPtr>                   TrFuncPtrVec;

TrFuncPtrVec tr_func_ptr_vec =
    {
        &Triangle::dummy_update_sides,
        &Triangle::init_ha_hb_hc,
        &Triangle::init_la_lb_lc,
        &Triangle::init_ma_mb_mc,
        &Triangle::init_sa_sb_sc,
        &Triangle::init_ra_rb_rc
    };

class TriangleInfo
{
private:
    Triangle* tr;

public:
    TriangleInfo(Triangle * const _tr)
    {
        tr = _tr;
    }

    TrElemPtrMap get_tr_elem_ptr_map()
    {
        return {
            // Fundamental Elements.
            { "a", { &tr->a, 0 } },
            { "b", { &tr->b, 0 } },
            { "c", { &tr->c, 0 } },
            { "R", { &tr->R, 0 } },
            { "r", { &tr->r, 0 } },
            { "s", { &tr->s, 0 } },
            { "S", { &tr->S, 0 } },

            // Heights.
            { "ha", { &tr->ha, 1 } },
            { "hb", { &tr->hb, 1 } },
            { "hc", { &tr->hc, 1 } },

            // Bisectors.
            { "la", { &tr->la, 2 } },
            { "lb", { &tr->lb, 2 } },
            { "lc", { &tr->lc, 2 } },

            // Medians.
            { "ma", { &tr->ma, 3 } },
            { "mb", { &tr->mb, 3 } },
            { "mc", { &tr->mc, 3 } },

            // Symmedians.
            { "sa", { &tr->sa, 4 } },
            { "sb", { &tr->sb, 4 } },
            { "sc", { &tr->sc, 4 } },

            // Excircles radii.
            { "ra", { &tr->ra, 5 } },
            { "rb", { &tr->rb, 5 } },
            { "rc", { &tr->rc, 5 } }
        };
    }
};

#endif /* triangle_info */
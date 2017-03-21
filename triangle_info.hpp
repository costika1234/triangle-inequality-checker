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
        &Triangle::init_ra_rb_rc,
        &Triangle::init_HA_HB_HC,
        &Triangle::init_IA_IB_IC,
        &Triangle::init_GA_GB_GC,
        &Triangle::init_sinA_sinB_sinC,
        &Triangle::init_sin2A_sin2B_sin2C,
        &Triangle::init_cosA_cosB_cosC,
        &Triangle::init_cos2A_cos2B_cos2C,
        &Triangle::init_cos3A_cos3B_cos3C,
        &Triangle::init_tanA_tanB_tanC,
        &Triangle::init_sinA2_sinB2_sinC2,
        &Triangle::init_cosA2_cosB2_cosC2,
        &Triangle::init_tanA2_tanB2_tanC2,
        &Triangle::init_tanA4_tanB4_tanC4,
        &Triangle::init_K,
        &Triangle::init_Q
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
            { "rc", { &tr->rc, 5 } },

            // Orthocenter to vertices.
            { "HA", { &tr->HA, 6 } },
            { "HB", { &tr->HB, 6 } },
            { "HC", { &tr->HC, 6 } },

            // Incenter to vertices
            { "IA", { &tr->IA, 7 } },
            { "IB", { &tr->IB, 7 } },
            { "IC", { &tr->IC, 7 } },

            // Centroid to vertices.
            { "GA", { &tr->GA, 8 } },
            { "GB", { &tr->GB, 8 } },
            { "GC", { &tr->GC, 8 } },

            // Half sines.
            { "sinA2", { &tr->sinA2, 15 } },
            { "sinB2", { &tr->sinB2, 15 } },
            { "sinC2", { &tr->sinC2, 15 } },

            // Half cosines.
            { "cosA2", { &tr->cosA2, 16 } },
            { "cosB2", { &tr->cosB2, 16 } },
            { "cosC2", { &tr->cosC2, 16 } },

            // Sum of half sines and half cosines.
            { "K", { &tr->K, 19 } },
            { "Q", { &tr->Q, 20 } }
        };
    }
};

#endif /* triangle_info */
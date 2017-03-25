#ifndef triangle_hpp
#define triangle_info

#include "triangle.hpp"

using namespace std;

typedef void (Triangle::*TrMemberFuncPtr)();
typedef unordered_map<string, pair<long_d*, int>> TrElemPtrMap;
typedef vector<TrMemberFuncPtr>                   TrFuncPtrVec;

class TriangleInfo
{
private:
    Triangle* tr;

public:
    TriangleInfo(Triangle * const _tr)
    {
        tr = _tr;
    }

    TrFuncPtrVec get_tr_func_ptr_vec()
    {
        return {
            &Triangle::init_sinA_sinB_sinC,
            &Triangle::init_sin2A_sin2B_sin2C,
            &Triangle::init_sin3A_sin3B_sin3C,
            &Triangle::init_cosA_cosB_cosC,
            &Triangle::init_cos2A_cos2B_cos2C,
            &Triangle::init_cos3A_cos3B_cos3C,
            &Triangle::init_tanA_tanB_tanC,
            &Triangle::init_sinA2_sinB2_sinC2,
            &Triangle::init_cosA2_cosB2_cosC2,
            &Triangle::init_tanA2_tanB2_tanC2,
            &Triangle::init_tanA4_tanB4_tanC4,
            &Triangle::init_K,
            &Triangle::init_Q,
            &Triangle::init_ha_hb_hc,
            &Triangle::init_la_lb_lc,
            &Triangle::init_ma_mb_mc,
            &Triangle::init_sa_sb_sc,
            &Triangle::init_ra_rb_rc,
            &Triangle::init_HA_HB_HC,
            &Triangle::init_IA_IB_IC,
            &Triangle::init_GA_GB_GC,
            &Triangle::dummy_update_sides
        };
    }

    TrElemPtrMap get_tr_elem_ptr_map()
    {
        return {
            { "sinA", { &tr->sinA, 0 } },
            { "sinB", { &tr->sinB, 0 } },
            { "sinC", { &tr->sinC, 0 } },
            
            { "sin2A", { &tr->sin2A, 1 } },
            { "sin2B", { &tr->sin2B, 1 } },
            { "sin2C", { &tr->sin2C, 1 } },
            
            { "sin3A", { &tr->sin3A, 2 } },
            { "sin3B", { &tr->sin3B, 2 } },
            { "sin3C", { &tr->sin3C, 2 } },
            
            { "cosA", { &tr->cosA, 3 } },
            { "cosB", { &tr->cosB, 3 } },
            { "cosC", { &tr->cosC, 3 } },
            
            { "cos2A", { &tr->cos2A, 4 } },
            { "cos2B", { &tr->cos2B, 4 } },
            { "cos2C", { &tr->cos2C, 4 } },
            
            { "cos3A", { &tr->cos3A, 5 } },
            { "cos3B", { &tr->cos3B, 5 } },
            { "cos3C", { &tr->cos3C, 5 } },
            
            { "tanA", { &tr->tanA, 6 } },
            { "tanB", { &tr->tanB, 6 } },
            { "tanC", { &tr->tanC, 6 } },
            
            { "sinA2", { &tr->sinA2, 7 } },
            { "sinB2", { &tr->sinB2, 7 } },
            { "sinC2", { &tr->sinC2, 7 } },
            
            { "cosA2", { &tr->cosA2, 8 } },
            { "cosB2", { &tr->cosB2, 8 } },
            { "cosC2", { &tr->cosC2, 8 } },
            
            { "tanA2", { &tr->tanA2, 9 } },
            { "tanB2", { &tr->tanB2, 9 } },
            { "tanC2", { &tr->tanC2, 9 } },
            
            { "tanA4", { &tr->tanA4, 10 } },
            { "tanB4", { &tr->tanB4, 10 } },
            { "tanC4", { &tr->tanC4, 10 } },
            
            { "K", { &tr->K, 11 } },
            
            { "Q", { &tr->Q, 12 } },
            
            { "ha", { &tr->ha, 13 } },
            { "hb", { &tr->hb, 13 } },
            { "hc", { &tr->hc, 13 } },
            
            { "la", { &tr->la, 14 } },
            { "lb", { &tr->lb, 14 } },
            { "lc", { &tr->lc, 14 } },
            
            { "ma", { &tr->ma, 15 } },
            { "mb", { &tr->mb, 15 } },
            { "mc", { &tr->mc, 15 } },
            
            { "sa", { &tr->sa, 16 } },
            { "sb", { &tr->sb, 16 } },
            { "sc", { &tr->sc, 16 } },
            
            { "ra", { &tr->ra, 17 } },
            { "rb", { &tr->rb, 17 } },
            { "rc", { &tr->rc, 17 } },
            
            { "HA", { &tr->HA, 18 } },
            { "HB", { &tr->HB, 18 } },
            { "HC", { &tr->HC, 18 } },
            
            { "IA", { &tr->IA, 19 } },
            { "IB", { &tr->IB, 19 } },
            { "IC", { &tr->IC, 19 } },
            
            { "GA", { &tr->GA, 20 } },
            { "GB", { &tr->GB, 20 } },
            { "GC", { &tr->GC, 20 } },
            
            { "a", { &tr->a, 21 } },
            { "b", { &tr->b, 21 } },
            { "c", { &tr->c, 21 } },
            { "R", { &tr->R, 21 } },
            { "r", { &tr->r, 21 } },
            { "s", { &tr->s, 21 } },
            { "S", { &tr->S, 21 } }
        };
    }
};

#endif /* triangle_info */
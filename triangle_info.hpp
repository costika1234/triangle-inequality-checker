#include "triangle.hpp"

typedef void (Triangle::*TrMemberFuncPtr)();
typedef unordered_map<string, pair<long_d*, string>> TrElemPtrMap;
typedef unordered_map<string, TrMemberFuncPtr> TrFuncPtrMap;
typedef vector<TrMemberFuncPtr> TrFuncPtrVec;

TrFuncPtrMap tr_func_ptr_map = {
    { "ma_mb_mc", &Triangle::init_ma_mb_mc },
    { "ha_hb_hc", &Triangle::init_ha_hb_hc }
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
            { "ma", { &tr->ma, "ma_mb_mc" } },
            { "mb", { &tr->mb, "ma_mb_mc" } },
            { "mc", { &tr->mc, "ma_mb_mc" } },
            { "ha", { &tr->ha, "ha_hb_hc" } },
            { "hb", { &tr->hb, "ha_hb_hc" } },
            { "hc", { &tr->hc, "ha_hb_hc" } }
        };
    }
};
#ifndef triangle_hpp
#define triangle_hpp

#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
#include <cfloat>
#include <unordered_map>
#include <stdio.h>

#include "utils.h"

using namespace std;

typedef tuple<long_d, long_d, long_d> tuple_3;
typedef long_d (*trig_func)(long_d);

class Triangle
{
public:
    // Sides.
    long_d a, b, c;

    // Angles.
    long_d A, B, C;

    // Fundamental elements.
    long_d R, r, s;

    // Area.
    long_d S;

    // Altitudes.
    long_d ha, hb, hc;

    // Bisectors.
    long_d la, lb, lc;

    // Medians.
    long_d ma, mb, mc;

    // Symmedians.
    long_d sa, sb, sc;

    // Radii of excircles.
    long_d ra, rb, rc;

    // HA, HB, HC -- H = orthocenter.
    long_d HA, HB, HC;

    // IA, IB, IC -- I = incenter.
    long_d IA, IB, IC;

    // GA, GB, GC -- G = centroid.
    long_d GA, GB, GC;

    // Sines.
    long_d sinA, sinB, sinC;

    // Double sines.
    long_d sin2A, sin2B, sin2C;

    // Cosines.
    long_d cosA, cosB, cosC;

    // Double cosines.
    long_d cos2A, cos2B, cos2C;

    // Triple cosines.
    long_d cos3A, cos3B, cos3C;

    // Tangents.
    long_d tanA, tanB, tanC;

    // Cotangents.
    long_d cotA, cotB, cotC;

    // Double tangents.
    long_d tan2A, tan2B, tan2C;

    // Half sines.
    long_d sinA2, sinB2, sinC2;

    // Half cosines
    long_d cosA2, cosB2, cosC2;

    // Half tangents.
    long_d tanA2, tanB2, tanC2;

    // Quarter tangents.
    long_d tanA4, tanB4, tanC4;

    // K = sum(sin(A/2)); Q = sum(cos(A/2));
    long_d K, Q;

    Triangle(long_d _a, long_d _b, long_d _c);

    void update_sides(long_d _a, long_d _b, long_d _c);

    void dummy_update_sides();

    void init_angles();

    void init_Rrs();

    long_d get_max_angle();

    long_d get_min_angle();

    bool is_bager_type_I();

    void init_sinA_sinB_sinC();

    void init_sin2A_sin2B_sin2C();

    void init_cosA_cosB_cosC();

    void init_cos2A_cos2B_cos2C();

    void init_cos3A_cos3B_cos3C();

    void init_tanA_tanB_tanC();

    void init_sinA2_sinB2_sinC2();

    void init_cosA2_cosB2_cosC2();

    void init_tanA2_tanB2_tanC2();

    void init_cotA2_cotB2_cotC2();

    tuple_3 get_trig_func_of_nA_nB_nC(trig_func func, long_d n);

    void init_K();

    void init_Q();

    void init_ha_hb_hc();

    void init_la_lb_lc();

    void init_ma_mb_mc();

    void init_sa_sb_sc();

    void init_ra_rb_rc();

    void init_HA_HB_HC();

    void init_IA_IB_IC();

    void init_GA_GB_GC();
};

#endif /* triangle_hpp */

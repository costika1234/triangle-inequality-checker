#ifndef checker_hpp
#define checker_hpp

#include "triangle_info.hpp"
#include "print_utils.hpp"

#define exprtk_disable_caseinsensitivity
#include "exprtk.hpp"

typedef exprtk::symbol_table<long_d> symbol_table_t;
typedef exprtk::expression<long_d>     expression_t;
typedef exprtk::parser<long_d>             parser_t;

struct TriangleStats
{
    long      iterations, passes, failures, equality_cases;
    long      bager_I_passes, bager_II_passes;
    long      bager_I_iterations, bager_II_iterations;
    long_d    bager_I_pass_rate, bager_II_pass_rate, passed_rate;
    long_d    min_angle_holds, max_angle_holds;
    long_d    min_LHS, max_LHS, min_RHS, max_RHS;
    long_d    min_t, max_t;
    Triangle  tr_max_LHS, tr_min_RHS;

    TriangleStats(long_d phi_angle, long_d max_angle)
        : iterations(0), passes(0), failures(0), equality_cases(0)
        , bager_I_passes(0), bager_II_passes(0)
        , bager_I_iterations(0), bager_II_iterations(0)
        , bager_I_pass_rate(0), bager_II_pass_rate(0), passed_rate(0)
        , min_angle_holds(phi_angle), max_angle_holds(max_angle)
        , min_LHS(numeric_limits<long_d>::max()), max_LHS(numeric_limits<long_d>::min())
        , min_RHS(numeric_limits<long_d>::max()), max_RHS(numeric_limits<long_d>::min())
        , min_t(numeric_limits<long_d>::max()), max_t(numeric_limits<long_d>::min())
        , tr_max_LHS(Triangle(0, 0, 0)), tr_min_RHS(Triangle(0, 0, 0)) { }
};

class Checker
{
private:
    string       expanded_LHS, expanded_RHS;
    expression_t expression_LHS, expression_RHS;

    // min_angle <= min{A, B, C}.
    long_d min_angle;

    // max{A, B, C} <= max_angle.
    long_d max_angle;

    // max{A, B, C} >= phi_angle.
    long_d phi_angle;

    // Iteration step.
    long_d step;

    // Must not be both set to 'true'.
    const bool check_for_bager_I = false;
    const bool check_for_bager_II = false;

    // Other flags.
    const bool only_isosceles = false;
    const bool stop_if_false = false;
    const bool verbose = false;

    // Stats.
    TriangleStats stats;

    // Triangle instance.
    Triangle tr;

    // The necessary subset of triangle init functions to be called.
    TrFuncPtrVec required_init_funcs;

    static bool is_triangle(long_d a, long_d b, long_d c);

    void init_funcs_and_parser();

    void init_exprtk_parser(const string&        inequality_side,
                            expression_t&        expression,
                            const TrElemPtrMap&  tr_elem_ptr_map);

public:
    Checker(const string expanded_LHS,
            const string expanded_RHS,
            long_d _min_angle = 0,
            long_d _max_angle = 180,
            long_d _phi_angle = 0,
            long_d _step      = 0.1);

    TriangleStats get_stats() const;

    void run_range(long_d b_start, long_d b_end, long_d c_start, long_d c_end);
};

inline bool Checker::is_triangle(long_d a, long_d b, long_d c)
{
    return (a < b + c) && (b < c + a) && (c < a + b);
}

#endif /* checker_hpp */

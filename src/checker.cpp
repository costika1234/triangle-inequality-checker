#include <assert.h>
#include <future>

#include "checker.hpp"

#define CALL_MEMBER_FN(object, ptrToMember)  ((object).*(ptrToMember))

Checker::Checker(const string _inequality_LHS,
                 const string _inequality_RHS,
                 const vector<string> _constraints_LHS,
                 const vector<string> _constraints_RHS,
                 long_d _min_angle,
                 long_d _max_angle,
                 long_d _phi_angle,
                 long_d _step)
    : inequality_LHS(_inequality_LHS)
    , inequality_RHS(_inequality_RHS)
    , constraints_LHS(_constraints_LHS)
    , constraints_RHS(_constraints_RHS)
    , min_angle(_min_angle)
    , max_angle(_max_angle)
    , phi_angle(_phi_angle)
    , step(_step)
    , tr(Triangle(1, 1, 1))
    , stats(TriangleStats(_phi_angle, _max_angle))
{
    assert(min_angle <= min(phi_angle, max_angle) && phi_angle <= max_angle);

    // Initialize the necessary init functions to be called during the checker
    // run and set up the exprtk parser for both the inequality and constraints.
    init_funcs_and_parser(inequality_LHS,
                          inequality_RHS,
                          inequality_expr_LHS,
                          inequality_expr_RHS,
                          inequality_init_funcs);

    no_constraints = constraints_LHS.size();

    constraints_expr_LHS.resize(no_constraints);
    constraints_expr_RHS.resize(no_constraints);
    constraints_init_funcs.resize(no_constraints);

    for (int i = 0; i < no_constraints; ++i)
    {
        init_funcs_and_parser(constraints_LHS[i],
                              constraints_RHS[i],
                              constraints_expr_LHS[i],
                              constraints_expr_RHS[i],
                              constraints_init_funcs[i]);
    }
}

void Checker::init_funcs_and_parser(const string&  LHS,
                                    const string&  RHS,
                                    expression_t&  expr_LHS,
                                    expression_t&  expr_RHS,
                                    TrFuncPtrVec&  init_funcs)

{
    auto vars = get_vars_from_inequality(LHS, RHS);
    TriangleInfo tr_info(&tr);
    set<int> func_indices;
    const TrElemPtrMap tr_elem_ptr_map = tr_info.get_tr_elem_ptr_map();
    const TrFuncPtrVec tr_func_ptr_vec = tr_info.get_tr_func_ptr_vec();

    for (auto var : vars)
    {
        if (tr_elem_ptr_map.find(var) == tr_elem_ptr_map.end())
        {
            ostringstream exit_message;
            exit_message << "Variable '" << var
                         << "' is undefined in triangle-inequality-checker." << endl;
            throw runtime_error(exit_message.str());
        }
        func_indices.insert(tr_elem_ptr_map.find(var)->second.second);
    }

    // Remove the '0' index if present (as this corresponds to the dummy method).
    func_indices.erase(0);

    for (auto index : func_indices)
    {
        init_funcs.push_back(tr_func_ptr_vec[index]);
    }

    init_exprtk_parser(LHS, expr_LHS, tr_elem_ptr_map);
    init_exprtk_parser(RHS, expr_RHS, tr_elem_ptr_map);
}

void Checker::init_exprtk_parser(const string&       inequality_side,
                                 expression_t&       expression,
                                 const TrElemPtrMap& tr_elem_ptr_map)
{
    symbol_table_t symbol_table;
    parser_t       parser;

    for (auto var : get_vars_from_expression(inequality_side))
    {
        symbol_table.add_variable(var, *tr_elem_ptr_map.find(var)->second.first);
    }
    // Add 'exprtk' constants such as pi.
    symbol_table.add_constants();

    expression.register_symbol_table(symbol_table);
    parser.compile(inequality_side, expression);
}

void Checker::call_member_functions(const TrFuncPtrVec init_funcs)
{
    for (auto& init_func : init_funcs)
    {
        CALL_MEMBER_FN(tr, init_func)();
    }
}

void Checker::run_range(long_d b_start, long_d b_end, long_d c_start, long_d c_end)
{
    // Without loss of generality, set a = 100;
    long_d a = 100.0;

    for (long_d b = b_start ; b < b_end; b += step)
    {
        for (long_d c = c_start ; c < c_end; c += step)
        {
            if (!is_triangle(a, b, c))
            {
                continue;
            }

            if (only_isosceles && b != c)
            {
                continue;
            }

            tr.update_sides(a, b, c);

            if (check_for_bager_I && !tr.is_bager_type_I())
            {
                continue;
            }

            if (check_for_bager_II && tr.is_bager_type_I())
            {
                continue;
            }

            if (!(tr.get_min_angle() >= min_angle &&
                  tr.get_max_angle() <= max_angle &&
                  tr.get_max_angle() >= phi_angle))
            {
                continue;
            }

            // Check constraints.
            bool allConstraintsHold = true;
            for (int i = 0; i < no_constraints; ++i)
            {
                call_member_functions(constraints_init_funcs[i]);
                if (constraints_expr_LHS[i].value() > constraints_expr_RHS[i].value())
                {
                    allConstraintsHold = false;
                    break;
                }
            }

            if (!allConstraintsHold)
            {
                continue;
            }

            // Call only the necessary methods to initialize the triangle elements.
            call_member_functions(inequality_init_funcs);

            long_d t = tr.R / tr.r;
            long_d LHS = inequality_expr_LHS.value();
            long_d RHS = inequality_expr_RHS.value();

            stats.min_LHS = min(stats.min_LHS, LHS);
            stats.max_RHS = max(stats.max_RHS, RHS);

            // Record the triangle for which max_LHS is attained.
            if (LHS > stats.max_LHS)
            {
                stats.max_LHS = LHS;
                stats.tr_max_LHS.update_sides(a, b, c);
            }

            // Record the triangle for which min_RHS is attained.
            if (RHS < stats.min_RHS)
            {
                stats.min_RHS = RHS;
                stats.tr_min_RHS.update_sides(a, b, c);
            }

            // Count iterations.
            stats.iterations++;
            bool is_bager_type_I = tr.is_bager_type_I();

            if (is_bager_type_I)
            {
                stats.bager_I_iterations++;
            }
            else
            {
                stats.bager_II_iterations++;
            }

            if (verbose)
            {
                cout << tr;
                PrintUtils::display_LHS_RHS(LHS, RHS);
            }

            if (LHS > RHS)
            {
                if (almost_equal_relative(LHS, RHS))
                {
                    // Update counters and move to the next iteration.
                    stats.equality_cases++;
                    stats.passes++;
                    if (is_bager_type_I)
                    {
                        stats.bager_I_passes++;
                    }
                    else
                    {
                        stats.bager_II_passes++;
                    }
                    continue;
                }

                // The least interval for which the inequality surely holds.
                stats.min_angle_holds = max(stats.min_angle_holds, tr.get_min_angle());
                stats.max_angle_holds = min(stats.max_angle_holds, tr.get_max_angle());
                stats.failures++;

                if (stop_if_false)
                {
                    cout << tr;
                    PrintUtils::display_LHS_RHS(LHS, RHS);
                    return;
                }
            }
            else
            {
                stats.passes++;
                stats.min_t = min(stats.min_t, t);
                stats.max_t = max(stats.max_t, t);

                if (LHS == RHS)
                {
                    stats.equality_cases++;
                }

                // Update Bager counters.
                if (is_bager_type_I)
                {
                    stats.bager_I_passes++;
                }
                else
                {
                    stats.bager_II_passes++;
                }
            }
        }
    }
}

TriangleStats Checker::get_stats() const
{
    return stats;
}

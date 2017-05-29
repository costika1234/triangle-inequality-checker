#include <assert.h>
#include <future>

#include "checker.hpp"

#define CALL_MEMBER_FN(object, ptrToMember)  ((object).*(ptrToMember))

Checker::Checker(string _inequality,
                 long_d _min_angle,
                 long_d _max_angle,
                 long_d _phi_angle,
                 long_d _step)
    : inequality(_inequality)
    , min_angle(_min_angle)
    , max_angle(_max_angle)
    , phi_angle(_phi_angle)
    , step(_step)
    , tr(Triangle(1, 1, 1))
    , stats(TriangleStats(_phi_angle, _max_angle))
{
    assert(min_angle <= min(phi_angle, max_angle) && phi_angle <= max_angle);

    // Initialize the required init functions to be called during the checker
    // run and set up the exprtk parser.
    init_funcs_and_parser();
}

void Checker::init_funcs_and_parser()
{
    // Expand any cyclic sums/products.
    expand_cyclic_sums(inequality);
    expand_cyclic_products(inequality);

    // Parse inequality and extract LHS and RHS.
    auto sides = parse_inequality(inequality);
    expanded_LHS = get<0>(sides);
    expanded_RHS = get<1>(sides);
    auto all_vars = get_vars_from_inequality(expanded_LHS, expanded_RHS);

    TriangleInfo tr_info(&tr);
    set<int> func_indices;
    const TrElemPtrMap tr_elem_ptr_map = tr_info.get_tr_elem_ptr_map();
    const TrFuncPtrVec tr_func_ptr_vec = tr_info.get_tr_func_ptr_vec();

    for (auto var : all_vars)
    {
        func_indices.insert(tr_elem_ptr_map.find(var)->second.second);
    }

    // Remove the '0' index if present (as this corresponds to the dummy method).
    func_indices.erase(0);

    for (auto index : func_indices)
    {
        required_init_funcs.push_back(tr_func_ptr_vec[index]);
    }

    init_exprtk_parser(expanded_LHS, expression_LHS, tr_elem_ptr_map);
    init_exprtk_parser(expanded_RHS, expression_RHS, tr_elem_ptr_map);
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

    expression.register_symbol_table(symbol_table);
    parser.compile(inequality_side, expression);
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

            // Call only the necessary methods to initialize the triangle elements.
            for (auto& init_func : required_init_funcs)
            {
                CALL_MEMBER_FN(tr, init_func) ();
            }

            long_d t = tr.R / tr.r;
            long_d LHS = expression_LHS.value();
            long_d RHS = expression_RHS.value();

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

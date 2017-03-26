#include "checker.hpp"
#include "print_utils.hpp"

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
    TrElemPtrMap tr_elem_ptr_map = tr_info.get_tr_elem_ptr_map();
    TrFuncPtrVec tr_func_ptr_vec = tr_info.get_tr_func_ptr_vec();

    for_each(all_vars.begin(),
             all_vars.end(),
             [&](string var) { func_indices.insert(tr_elem_ptr_map[var].second); });

    // Remove the '0' index if present (as this corresponds to the dummy method).
    func_indices.erase(0);

    for_each(func_indices.begin(),
             func_indices.end(),
             [&](int index) { required_init_funcs.push_back(tr_func_ptr_vec[index]); });

    init_exprtk_parser(expanded_LHS, expression_LHS, tr_elem_ptr_map);
    init_exprtk_parser(expanded_RHS, expression_RHS, tr_elem_ptr_map);
}

void Checker::init_exprtk_parser(string        inequality_side,
                                 expression_t& expression,
                                 TrElemPtrMap& tr_elem_ptr_map)
{
    symbol_table_t symbol_table;
    parser_t       parser;

    set<string> vars = get_vars_from_expression(inequality_side);

    for (auto& var : vars)
    {
        string renamed_var = (elem_alias_map.find(var) != elem_alias_map.end())
            ? elem_alias_map[var]
            : var;
        symbol_table.add_variable(renamed_var, *tr_elem_ptr_map[var].first);
    }

    expression.register_symbol_table(symbol_table);

    // Before compiling the expression, replace all occurences of variables which
    // require aliases to prevent case sensitive clashes.
    string inequality_side_with_aliases = replace_vars_with_aliases(inequality_side);

    parser.compile(inequality_side_with_aliases, expression);
}

void Checker::run()
{
    // Without loss of generality, set a = 100;
    long_d a = 100.0, b = 0.0, c = 0.0;

    for (b = 0 ; b <= 1000; b += step)
    {
        for (c = 0 ; c <= 1000; c += step)
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
                display_LHS_RHS(LHS, RHS);
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
                    display_LHS_RHS(LHS, RHS);
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

void Checker::print_stats() const
{
    int underscore_length = 30 + string(expanded_LHS).length() + string(expanded_RHS).length();
    underscore_length = max(underscore_length, 65);
    string underscore_str = string(underscore_length, '_');
    set_precision(cout, VERY_LOW_PRECISION);

    cout << bold_on
         << " " + underscore_str << endl
         << "|" << endl
         << "|" << Color::FG_LIGHT_MAGENTA << " Checking inequality: " << Color::FG_WHITE << expanded_LHS
                                                                       << Color::FG_LIGHT_RED << " <= "
                                                                       << Color::FG_WHITE << expanded_RHS
                                                                       << Color::FG_DEFAULT << endl
         << "|" << endl
         << "|" << Color::FG_LIGHT_MAGENTA << " Subject to:          " << Color::FG_WHITE   << min_angle
                                                                       << " <= A, B, C <= " << max_angle
                                                                       << Color::FG_DEFAULT << endl
         << "|"                            << "                      " << Color::FG_WHITE << "max{A, B, C} >= "
                                                                       << phi_angle << Color::FG_DEFAULT << endl;

    if (display_min_max_triangles)
    {
        cout << "|" << endl
             << "|" << Color::FG_YELLOW << " (min_LHS, max_LHS) = " << "(" << stats.min_LHS << ", "
                                                                           << stats.max_LHS << ")"
                    << Color::FG_DEFAULT << endl
             << "|" << Color::FG_YELLOW << "   🔼  Triangle which maximizes LHS: " << Color::FG_DEFAULT << endl
                                                                                   << stats.tr_max_LHS
             << "|" << endl
             << "|" << Color::FG_YELLOW << " (min_RHS, max_RHS) = " << "(" << stats.min_RHS << ", "
                                                                           << stats.max_RHS << ")"
                    << Color::FG_DEFAULT << endl
             << "|" << Color::FG_YELLOW << "   🔼  Triangle which minimizes RHS: " << Color::FG_DEFAULT << endl
                                                                                   << stats.tr_min_RHS;
    }

    cout << "|" << endl
         << "| ❌ " << Color::FG_LIGHT_RED    << " failures:    " << stats.failures       << Color::FG_DEFAULT << endl
         << "| ✅ " << Color::FG_LIGHT_GREEN  << " passes:      " << stats.passes         << Color::FG_DEFAULT << endl
         << "| 🔶 " << Color::FG_LIGHT_YELLOW << " equalities:  " << stats.equality_cases << Color::FG_DEFAULT << endl
         << "| 🌀 " << Color::FG_DEFAULT      << " iterations:  " << stats.iterations     << Color::FG_DEFAULT << endl
         << "|" << endl
         << "|" << Color::FG_LIGHT_CYAN << " A >= B >= 60º >= C (Bager I):   "
                                        << 100 * (long_d) stats.bager_I_passes  / stats.bager_I_iterations
                                        << "%" << Color::FG_DEFAULT << endl
         << "|" << Color::FG_LIGHT_CYAN << " A >= 60º >= B >= C (Bager II):  "
                                        << 100 * (long_d) stats.bager_II_passes / stats.bager_II_iterations
                                        << "%" << Color::FG_DEFAULT << endl
         << "|" << endl;
    cout << "|" << Color::FG_GREEN << " Holds for angles in range:     ["
                                   << stats.min_angle_holds << "º, " << stats.max_angle_holds << "º]"
                                   << Color::FG_DEFAULT << endl;
    cout << "|" << Color::FG_GREEN << " Holds for t = R / r in range:  ["
                                   << stats.min_t << ", "  << stats.max_t << "]"
                                   << Color::FG_DEFAULT << endl
         << "|" << endl
         << "|" << Color::FG_WHITE << " PASSED: "
                                   << 100 * ((long_d) (stats.iterations - stats.failures)) / stats.iterations
                                   << "%" << Color::FG_DEFAULT << endl
         << "|" + underscore_str << endl
         << bold_off;
}

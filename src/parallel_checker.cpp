#include <limits>

#include "parallel_checker.hpp"

ParallelChecker::ParallelChecker(string inequality,
                                 long_d min_angle,
                                 long_d max_angle,
                                 long_d phi_angle,
                                 long_d step)
    : inequality(inequality)
    , min_angle(min_angle)
    , max_angle(max_angle)
    , phi_angle(phi_angle)
    , step(step)
    , finalStats(TriangleStats(phi_angle, max_angle))
{
    expand_cyclic_sums(inequality);
    expand_cyclic_products(inequality);

    auto sides = parse_inequality(inequality);
    expanded_LHS = get<0>(sides);
    expanded_RHS = get<1>(sides);

    futures.reserve(no_partitions);

    for (int i = 0; i < no_partitions; ++i)
    {
        checkers.push_back(new Checker(inequality, min_angle, max_angle, phi_angle, step));
        futures.push_back(async(launch::async, &Checker::run_range, checkers[i],
                                partitions[i][0], partitions[i][1], partitions[i][2], partitions[i][3]));
    }
}

TriangleStats ParallelChecker::get_stats()
{
    return finalStats;
}

void ParallelChecker::run()
{
    for (auto& future : futures)
    {
        future.get();
    }

    aggregate_stats();
}

void ParallelChecker::aggregate_stats()
{
    for (int i = 0; i < no_partitions; ++i)
    {
        TriangleStats checker_stats = checkers[i]->get_stats();

        finalStats.iterations     += checker_stats.iterations;
        finalStats.passes         += checker_stats.passes;
        finalStats.failures       += checker_stats.failures;
        finalStats.equality_cases += checker_stats.equality_cases;

        finalStats.bager_I_passes      += checker_stats.bager_I_passes;
        finalStats.bager_II_passes     += checker_stats.bager_II_passes;
        finalStats.bager_I_iterations  += checker_stats.bager_I_iterations;
        finalStats.bager_II_iterations += checker_stats.bager_II_iterations;

        finalStats.min_angle_holds = max(finalStats.min_angle_holds, checker_stats.min_angle_holds);
        finalStats.max_angle_holds = min(finalStats.max_angle_holds, checker_stats.max_angle_holds);

        finalStats.min_LHS = min(finalStats.min_LHS, checker_stats.min_LHS);
        finalStats.max_LHS = max(finalStats.max_LHS, checker_stats.max_LHS);
        finalStats.min_RHS = min(finalStats.min_RHS, checker_stats.min_RHS);
        finalStats.max_RHS = max(finalStats.max_RHS, checker_stats.max_RHS);

        finalStats.min_t = min(finalStats.min_t, checker_stats.min_t);
        finalStats.max_t = max(finalStats.max_t, checker_stats.max_t);

        // Reasonable to assume that max_t = inf when value is large enough.
        if (finalStats.max_t > MAX_T_THRESHOLD)
        {
            finalStats.max_t = numeric_limits<long_d>::infinity();
        }

        if (finalStats.max_LHS == checker_stats.max_LHS)
        {
            finalStats.tr_max_LHS = checker_stats.tr_max_LHS;
        }

        if (finalStats.min_RHS == checker_stats.min_RHS)
        {
            finalStats.tr_min_RHS = checker_stats.tr_min_RHS;
        }
    }

    finalStats.bager_I_pass_rate = 100 * (long_d) finalStats.bager_I_passes  / finalStats.bager_I_iterations;
    finalStats.bager_II_pass_rate = 100 * (long_d) finalStats.bager_II_passes / finalStats.bager_II_iterations;
    finalStats.passed_rate = 100 * ((long_d) (finalStats.iterations - finalStats.failures)) / finalStats.iterations;
}

void ParallelChecker::display_stats()
{
    int underscore_length = 30 + string(expanded_LHS).length() + string(expanded_RHS).length();
    underscore_length = max(underscore_length, 65);
    string underscore_str = string(underscore_length, '_');
    PrintUtils::set_precision(cout, VERY_LOW_PRECISION);

    cout << PrintUtils::bold_on
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
             << "|" << Color::FG_YELLOW << " (min_LHS, max_LHS) = " << "(" << finalStats.min_LHS << ", "
                                                                           << finalStats.max_LHS << ")"
                    << Color::FG_DEFAULT << endl
             << "|" << Color::FG_YELLOW << "   🔼  Triangle which maximizes LHS: " << Color::FG_DEFAULT << endl
                                                                                   << finalStats.tr_max_LHS
             << "|" << endl
             << "|" << Color::FG_YELLOW << " (min_RHS, max_RHS) = " << "(" << finalStats.min_RHS << ", "
                                                                           << finalStats.max_RHS << ")"
                    << Color::FG_DEFAULT << endl
             << "|" << Color::FG_YELLOW << "   🔼  Triangle which minimizes RHS: " << Color::FG_DEFAULT << endl
                                                                                   << finalStats.tr_min_RHS;
    }

    cout << "|" << endl
         << "| ❌ " << Color::FG_LIGHT_RED    << " failures:    " << finalStats.failures       << Color::FG_DEFAULT << endl
         << "| ✅ " << Color::FG_LIGHT_GREEN  << " passes:      " << finalStats.passes         << Color::FG_DEFAULT << endl
         << "| 🔶 " << Color::FG_LIGHT_YELLOW << " equalities:  " << finalStats.equality_cases << Color::FG_DEFAULT << endl
         << "| 🌀 " << Color::FG_DEFAULT      << " iterations:  " << finalStats.iterations     << Color::FG_DEFAULT << endl
         << "|" << endl
         << "|" << Color::FG_LIGHT_CYAN << " A >= B >= 60º >= C (Bager I):   "
                                        << finalStats.bager_I_pass_rate
                                        << "%" << Color::FG_DEFAULT << endl
         << "|" << Color::FG_LIGHT_CYAN << " A >= 60º >= B >= C (Bager II):  "
                                        << finalStats.bager_II_pass_rate
                                        << "%" << Color::FG_DEFAULT << endl
         << "|" << endl;
    cout << "|" << Color::FG_GREEN << " Holds for angles in range:     ["
                                   << finalStats.min_angle_holds << "º, " << finalStats.max_angle_holds << "º]"
                                   << Color::FG_DEFAULT << endl;
    cout << "|" << Color::FG_GREEN << " Holds for t = R / r in range:  ["
                                   << finalStats.min_t << ", "  << finalStats.max_t << "]"
                                   << Color::FG_DEFAULT << endl
         << "|" << endl
         << "|" << Color::FG_WHITE << " PASSED: "
                                   << finalStats.passed_rate
                                   << "%" << Color::FG_DEFAULT << endl
         << "|" + underscore_str << endl
         << PrintUtils::bold_off;
}


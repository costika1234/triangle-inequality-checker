#include <cmath>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cfloat>
#include <assert.h>

#include "inequality.h"
#include "triangle.hpp"
#include "utils.h"

#define VERY_LOW_PRECISION  5
#define LOW_PRECISION      15
#define HIGH_PRECISION     30
#define NO_ITERATIONS      20

using namespace std;
    
namespace Color 
{
    enum Code 
    {
        FG_RED           = 31,
        FG_GREEN         = 32,
        FG_YELLOW        = 33,
        FG_BLUE          = 34,
        FG_MAGENTA       = 35, 
        FG_CYAN          = 36, 
        FG_LIGHT_GRAY    = 37,
        FG_DEFAULT       = 39, 
        FG_DARK_GRAY     = 90, 
        FG_LIGHT_RED     = 91, 
        FG_LIGHT_GREEN   = 92, 
        FG_LIGHT_YELLOW  = 93, 
        FG_LIGHT_BLUE    = 94, 
        FG_LIGHT_MAGENTA = 95, 
        FG_LIGHT_CYAN    = 96, 
        FG_WHITE         = 97
    };

    std::ostream& operator<<(std::ostream& os, Code code) 
    {
        return os << "\033[" << static_cast<int>(code) << "m";
    }
}

void set_precision(std::ostream &stream, int precision)
{
    stream.precision(precision);
    stream.setf(ios::fixed, ios::floatfield);
}

ostream& operator<<(ostream& os, const Triangle& tr)
{
    os << "|     Angles: (" << tr.A << "º, " << tr.B << "º, " << tr.C << "º)" << endl
       << "|     Sides:  (" << tr.a << ", "  << tr.b << ", "  << tr.c << ")"  << endl;
    return os;
}

std::ostream& bold_on(std::ostream& os)
{
    return os << "\e[1m";
}

std::ostream& bold_off(std::ostream& os)
{
    return os << "\e[0m";
}

bool is_triangle(long_d a,
                 long_d b,
                 long_d c)
{
    return (a < b + c) && (b < c + a) && (c < a + b);
}

void display_LHS_RHS(long_d LHS,
                     long_d RHS)
{
    string ineq_holds_str = (LHS <= RHS) ? "✅" : "❌";
    cout << "(LHS: " << LHS << ", RHS: " << RHS << ") " << ineq_holds_str << endl << endl;
}

void print_stats(long_d   min_angle,
                 long_d   max_angle,
                 long_d   phi_angle,
                 long_d   min_LHS,
                 long_d   max_LHS,
                 long_d   min_RHS,
                 long_d   max_RHS,
                 int      iterations,
                 int      passes,
                 int      failures,
                 int      equality_cases,
                 int      bager_I_passes,
                 int      bager_I_iterations,
                 int      bager_II_passes,
                 int      bager_II_iterations,
                 long_d   min_angle_holds,
                 long_d   max_angle_holds,
                 Triangle t_max_LHS,
                 Triangle t_min_RHS,
                 long_d   min_t,
                 long_d   max_t,
                 bool     print_min_max_triangles)
{
    int underscore_length = 30 + string(LHS_str).length() + string(RHS_str).length();
    underscore_length = max(underscore_length, 65);
    string underscore_str = string(underscore_length, '_');
    set_precision(cout, VERY_LOW_PRECISION);

    cout << bold_on
         << " " + underscore_str << endl
         << "|" << endl
         << "|" << Color::FG_LIGHT_MAGENTA << " Checking inequality: " << Color::FG_WHITE << LHS_str << Color::FG_LIGHT_RED << " <= " << Color::FG_WHITE << RHS_str << Color::FG_DEFAULT << endl
         << "|" << endl
         << "|" << Color::FG_LIGHT_MAGENTA << " Subject to:          " << Color::FG_WHITE << min_angle << " <= A, B, C <= " << max_angle << Color::FG_DEFAULT << endl
         << "|"                            << "                      " << Color::FG_WHITE << "max{A, B, C} >= " << phi_angle << Color::FG_DEFAULT << endl;
    
    if (print_min_max_triangles)
    {
        cout << "|" << endl
             << "|" << Color::FG_YELLOW << " (min_LHS, max_LHS) = " << "(" << min_LHS << ", " << max_LHS << ")" << Color::FG_DEFAULT << endl
             << "|" << Color::FG_YELLOW << "   🔼  Triangle which maximizes LHS: " << Color::FG_DEFAULT << endl << t_max_LHS
             << "|" << endl
             << "|" << Color::FG_YELLOW << " (min_RHS, max_RHS) = " << "(" << min_RHS << ", " << max_RHS << ")" << Color::FG_DEFAULT << endl
             << "|" << Color::FG_YELLOW << "   🔼  Triangle which minimizes RHS: " << Color::FG_DEFAULT << endl << t_min_RHS;
    } 
        
    cout << "|" << endl
         << "| ❌ " << Color::FG_LIGHT_RED    << " failures:    " << failures       << Color::FG_DEFAULT << endl
         << "| ✅ " << Color::FG_LIGHT_GREEN  << " passes:      " << passes         << Color::FG_DEFAULT << endl
         << "| 🔶 " << Color::FG_LIGHT_YELLOW << " equalities:  " << equality_cases << Color::FG_DEFAULT << endl
         << "| 🌀 " << Color::FG_DEFAULT      << " iterations:  " << iterations     << Color::FG_DEFAULT << endl
         << "|" << endl
         << "|" << Color::FG_LIGHT_CYAN << " A >= B >= 60º >= C (Bager I):   " << 100 * (long_d) bager_I_passes  / bager_I_iterations  << "%" << Color::FG_DEFAULT << endl
         << "|" << Color::FG_LIGHT_CYAN << " A >= 60º >= B >= C (Bager II):  " << 100 * (long_d) bager_II_passes / bager_II_iterations << "%" << Color::FG_DEFAULT << endl
         << "|" << endl
         << "|" << Color::FG_GREEN << " Holds for angles in range:     [" << min_angle_holds << "º, " << max_angle_holds << "º]" << Color::FG_DEFAULT << endl
         << "|" << Color::FG_GREEN << " Holds for t = R / r in range:  [" << min_t           << ", "  << max_t           << "]"  << Color::FG_DEFAULT << endl
         << "|" << endl
         << "|" << Color::FG_WHITE << " PASSED: " << 100 * ((long_d) (iterations - failures)) / iterations << "%" << Color::FG_DEFAULT << endl
         << "|" + underscore_str << endl
         << bold_off;
}

// General purpose inequality checker.
bool check_inequality(long_d step,
                      long_d min_angle,
                      long_d max_angle,
                      long_d phi_angle,
                      bool   check_for_bager_I,
                      bool   check_for_bager_II,
                      bool   only_isosceles,
                      bool   stop_if_false,
                      bool   verbose,
                      bool   print_min_max_triangles)
{
    // Without loss of generality, set a = 100;
    long_d a = 100.0;
    long_d b = 0.0;
    long_d c = 0.0;
    
    int iterations = 0;
    int passes = 0;
    int failures = 0;
    int equality_cases = 0;
    int bager_I_passes = 0;
    int bager_II_passes = 0;
    int bager_I_iterations = 0;
    int bager_II_iterations = 0;
    long_d min_LHS =  MAXFLOAT, min_RHS =  MAXFLOAT, min_angle_holds = phi_angle;
    long_d max_LHS = -MAXFLOAT, max_RHS = -MAXFLOAT, max_angle_holds = max_angle;
    long_d min_t   =  MAXFLOAT, max_t   = -MAXFLOAT;
    bool holds = true;
    Triangle tr_max_LHS(0, 0, 0);
    Triangle tr_min_RHS(0, 0, 0);
    
    for (b = 0; b <= 1000; b += step)
    {
        for (c = 0; c <= 1000; c += step)
        {
            if (!is_triangle(a, b, c))
            {
                continue;
            }

            if (only_isosceles && b != c)
            {
                continue;
            }
            
            Triangle tr(a, b, c);

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
            
            INIT_VARS;
            long_d t = tr.R / tr.r;
            long_d LHS = LHS_expr;
            long_d RHS = RHS_expr;
            
            min_LHS = min(min_LHS, LHS);
            max_RHS = max(max_RHS, RHS);
            
            // Record the triangle for which max_LHS is attained.
            if (LHS > max_LHS)
            {
                max_LHS = LHS;
                tr_max_LHS = Triangle(a, b, c);
            }
            
            // Record the triangle for which min_RHS is attained.
            if (RHS < min_RHS)
            {
                min_RHS = RHS;
                tr_min_RHS = Triangle(a, b, c);
            }
            
            // Count iterations.
            iterations++;
            bool is_bager_type_I = tr.is_bager_type_I();

            if (is_bager_type_I)
            {
                bager_I_iterations++;
            }
            else
            {
                bager_II_iterations++;
            }
            
            if (LHS > RHS)
            {
                if (almost_equal_relative(LHS, RHS))
                {
                    // Update counters and move to the next iteration.
                    equality_cases++;
                    passes++;
                    if (is_bager_type_I)
                    {
                        bager_I_passes++;
                    }
                    else
                    {
                        bager_II_passes++;
                    }
                    continue;
                }

                // The least interval for which the inequality surely holds.
                min_angle_holds = max(min_angle_holds, tr.get_min_angle());
                max_angle_holds = min(max_angle_holds, tr.get_max_angle());
                holds = false;
                failures++;
                
                if (verbose)
                {
                    cout << tr;
                    display_LHS_RHS(LHS, RHS);
                }

                if (stop_if_false)
                {
                    cout << tr;
                    display_LHS_RHS(LHS, RHS);
                    return false;
                }
            }
            else
            {
                passes++;
                min_t = min(min_t, t);
                max_t = max(max_t, t);

                if (LHS == RHS)
                {
                    equality_cases++;
                }

                // Update Bager counters.
                if (is_bager_type_I)
                {
                    bager_I_passes++;
                }
                else
                {
                    bager_II_passes++;
                }
            }
        }
    }

    print_stats(min_angle,
                max_angle,
                phi_angle,
                min_LHS,
                max_LHS,
                min_RHS,
                max_RHS,
                iterations,
                passes,
                failures,
                equality_cases,
                bager_I_passes,
                bager_I_iterations,
                bager_II_passes,
                bager_II_iterations,
                min_angle_holds,
                max_angle_holds,
                tr_max_LHS,
                tr_min_RHS,
                min_t,
                max_t,
                print_min_max_triangles);
    
    return holds;
}

int main(int argc, const char * argv[])
{
    // min{A, B, C} >= min_angle.
    long_d min_angle = 0;
    // max{A, B, C} <= max_angle.
    long_d max_angle = 180;
    // max{A, B, C} >= phi_angle.
    long_d phi_angle = 0;
    // Iteration step. 
    long_d step = 0.1;

    assert(min_angle <= min(phi_angle, max_angle) && phi_angle <= max_angle);

    // Must not be both set to 'true'.
    bool check_for_bager_I = false;
    bool check_for_bager_II = false;

    // Other flags.
    bool only_isosceles = false;
    bool stop_if_false = true;
    bool verbose = false;
    bool print_min_max_triangles = false;

    check_inequality(step,
                     min_angle,
                     max_angle,
                     phi_angle,
                     check_for_bager_I,
                     check_for_bager_II,
                     only_isosceles,
                     stop_if_false,
                     verbose,
                     print_min_max_triangles);

    return 0;
}

#ifndef utils_hpp
#define utils_hpp

#include <stdio.h>
#include <cmath>
#include <regex>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <cfloat>
#include <set>

#include "constants.hpp"

typedef long double long_d;

template<class BidirIt, class Traits, class CharT, class UnaryFunction>
basic_string<CharT> regex_replace_with_callback(BidirIt first,
                                                BidirIt last,
                                                const basic_regex<CharT, Traits>& re,
                                                UnaryFunction f)
{
    basic_string<CharT> s;

    typename match_results<BidirIt>::difference_type positionOfLastMatch = 0;
    auto endOfLastMatch = first;

    auto callback = [&](const match_results<BidirIt>& match)
    {
        auto positionOfThisMatch = match.position(0);
        auto diff = positionOfThisMatch - positionOfLastMatch;

        auto startOfThisMatch = endOfLastMatch;
        advance(startOfThisMatch, diff);

        s.append(endOfLastMatch, startOfThisMatch);
        s.append(f(match));

        auto lengthOfMatch = match.length(0);

        positionOfLastMatch = positionOfThisMatch + lengthOfMatch;

        endOfLastMatch = startOfThisMatch;
        advance(endOfLastMatch, lengthOfMatch);
    };

    sregex_iterator begin(first, last, re), end;
    for_each(begin, end, callback);

    s.append(endOfLastMatch, last);

    return s;
}

template<class Traits, class CharT, class UnaryFunction>
string regex_replace_with_callback(const string& s,
                                   const basic_regex<CharT, Traits>& re,
                                   UnaryFunction f)
{
    return regex_replace_with_callback(s.cbegin(), s.cend(), re, f);
}

// Trim from start.
static inline string& ltrim(string &s)
{
    s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
    return s;
}

// Trim from end.
static inline string& rtrim(string &s)
{
    s.erase(find_if(s.rbegin(), s.rend(),
            not1(ptr_fun<int, int>(isspace))).base(), s.end());
    return s;
}

// Trim from both ends.
static inline string& trim(string &s)
{
    return ltrim(rtrim(s));
}

static bool almost_equal_relative(long_d A,
                                  long_d B,
                                  float maxRelDiff = FLT_EPSILON)
{
    // Calculate the difference.
    long_d diff = fabs(A - B);
    A = fabs(A);
    B = fabs(B);

    // Find the largest
    long_d largest = (B > A) ? B : A;

    return (diff <= largest * maxRelDiff);
}

// Numerically stable version of cotangent function.
static long_d cot(long_d x)
{
    return cos(x) / sin(x);
}

static bool is_matching_brackets(string s)
{
    return count(s.begin(), s.end(), '(') == count(s.begin(), s.end(), ')') &&
           count(s.begin(), s.end(), '[') == count(s.begin(), s.end(), ']');
}

static bool is_distance_between_remarkable_points(const string& match)
{
    if (match.length() == 2)
    { 
        regex remarkable_distance_regex(REGEX_REMARKABLE_DIST);
        return sregex_iterator(match.begin(), match.end(), remarkable_distance_regex) != sregex_iterator();
    }
    
    return false;
}

static set<string> get_vars_from_expression(const string& expr)
{
    set<string> result;

    regex triangle_var_regex(REGEX_TRIANGLE_VAR);
    auto triangle_vars_it = sregex_iterator(expr.begin(), expr.end(), triangle_var_regex);

    for (auto it = triangle_vars_it; it != sregex_iterator(); ++it)
    {
        result.insert(it->str());
    }

    return result;
}

static set<string> get_vars_from_inequality(const string& LHS, const string& RHS)
{
    set<string> lhs_vars = get_vars_from_expression(LHS);
    set<string> rhs_vars = get_vars_from_expression(RHS);

    set<string> all_vars;
    set_union(lhs_vars.begin(),
              lhs_vars.end(),
              rhs_vars.begin(),
              rhs_vars.end(),
              inserter(all_vars, all_vars.begin()));

    return all_vars;
}

static void replace_char_with_next_cyclic_char(string& s, int pos, bool& has_replaced)
{
    if (tolower(s[pos]) >= 'a' && tolower(s[pos]) <= 'c')
    {
        s[pos] = (tolower(s[pos] + 1) > 'c') ? s[pos] - 2 : s[pos] + 1;
        has_replaced = true;
    }

    has_replaced = false;
}

static string get_next_var(const smatch& m)
{
    string var = m.str(0);
    bool has_replaced = false;
    replace_char_with_next_cyclic_char(var, var.length() - 1, has_replaced);

    if (!has_replaced)
    {
        replace_char_with_next_cyclic_char(var, var.length() - 2, has_replaced);
    }

    return var;
}

static string expand_cyclic_expr(const smatch& m, const string& op)
{
    string e1 = m.str(1);
    string e2 = regex_replace_with_callback(e1, regex(REGEX_TRIANGLE_VAR), &get_next_var);
    string e3 = regex_replace_with_callback(e2, regex(REGEX_TRIANGLE_VAR), &get_next_var);

    ostringstream cyclic_sum;
    cyclic_sum << "("
               << "(" << e1 << ") " << op << " "
               << "(" << e2 << ") " << op << " "
               << "(" << e3 << ")"
               << ")";

    return cyclic_sum.str();
}

static string expand_sum_expr(const smatch& m)
{
    return expand_cyclic_expr(m, "+");
}

static string expand_prod_expr(const smatch& m)
{
    return expand_cyclic_expr(m, "*");
}

static void expand_cyclic_sums(string& inequality)
{
    inequality = regex_replace_with_callback(inequality, regex(REGEX_CYCLIC_SUM), &expand_sum_expr);
}

static void expand_cyclic_products(string& inequality)
{
    inequality = regex_replace_with_callback(inequality, regex(REGEX_CYCLIC_PROD), &expand_prod_expr);
}

static pair<string, string> parse_inequality(string& inequality)
{
    if (!is_matching_brackets(inequality))
    {
        throw runtime_error("Invalid input: check brackets parity");
    }

    expand_cyclic_sums(inequality);
    expand_cyclic_products(inequality);

    string LHS, RHS;

    regex re(REGEX_LHS_LEQ_RHS);
    smatch match;

    if (regex_search(inequality, match, re) && match.size() > 1)
    {
        LHS = match.str(1);
        RHS = match.str(2);
    }
    else
    {
        regex re(REGEX_LHS_GEQ_RHS);
        if (regex_search(inequality, match, re) && match.size() > 1)
        {
            LHS = match.str(2);
            RHS = match.str(1);
        }
        else
        {
            throw runtime_error("Invalid input: missing either '<=' or '>='!");
        }
    }

    trim(LHS);
    trim(RHS);

    if (LHS.empty() || RHS.empty())
    {
        throw runtime_error("Invalid input: either LHS or RHS is empty!");
    }

    return pair<string, string>(LHS, RHS);
}

static long_d convert_radians_to_degrees(long_d angle_in_radians)
{
    return angle_in_radians * 180 / M_PI;
}

static long_d convert_degrees_to_radians(long_d angle_in_degrees)
{
    return angle_in_degrees * M_PI / 180;
}

#endif /* utils_hpp */

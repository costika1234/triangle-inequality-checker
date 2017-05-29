#ifndef utils_hpp
#define utils_hpp

#include <stdio.h>
#include <cmath>
#include <regex>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

#include "exprtk.hpp"
#include "aliases.hpp"

typedef long double long_d;
typedef exprtk::symbol_table<long_d> symbol_table_t;
typedef exprtk::expression<long_d>     expression_t;
typedef exprtk::parser<long_d>             parser_t;

namespace std
{

template<class BidirIt, class Traits, class CharT, class UnaryFunction>
std::basic_string<CharT> regex_replace(BidirIt first,
                                       BidirIt last,
                                       const std::basic_regex<CharT,Traits>& re,
                                       UnaryFunction f)
{
    std::basic_string<CharT> s;

    typename std::match_results<BidirIt>::difference_type positionOfLastMatch = 0;
    auto endOfLastMatch = first;

    auto callback = [&](const std::match_results<BidirIt>& match)
    {
        auto positionOfThisMatch = match.position(0);
        auto diff = positionOfThisMatch - positionOfLastMatch;

        auto startOfThisMatch = endOfLastMatch;
        std::advance(startOfThisMatch, diff);

        s.append(endOfLastMatch, startOfThisMatch);
        s.append(f(match));

        auto lengthOfMatch = match.length(0);

        positionOfLastMatch = positionOfThisMatch + lengthOfMatch;

        endOfLastMatch = startOfThisMatch;
        std::advance(endOfLastMatch, lengthOfMatch);
    };

    std::sregex_iterator begin(first, last, re), end;
    std::for_each(begin, end, callback);

    s.append(endOfLastMatch, last);

    return s;
}

template<class Traits, class CharT, class UnaryFunction>
std::string regex_replace(const std::string& s,
                          const std::basic_regex<CharT,Traits>& re,
                          UnaryFunction f)
{
    return regex_replace(s.cbegin(), s.cend(), re, f);
}

} // namespace std

using namespace std;

// Trim from start.
static inline std::string &ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// Trim from end.
static inline std::string &rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(),
            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// Trim from both ends.
static inline std::string &trim(std::string &s)
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

const string REGEX_LHS_LEQ_RHS    = "(.*)\\s*<=\\s*(.*)";
const string REGEX_LHS_GEQ_RHS    = "(.*)\\s*>=\\s*(.*)";
const string REGEX_INIT_FUNCTIONS = "void init_(.*)\\(\\);";
const string REGEX_TRIANGLE_VAR   = "(?!sqrt\\b|long_d\\b|t\\b|sum\\b|prod\\b|pow\\b)"
                                    "\\b[a-zA-Z]+([0-9]?)+[A-Z]?";
const string REGEX_CYCLIC_SUM     = "\\[sum (?!sum)([^\\[\\]]*)\\]";
const string REGEX_CYCLIC_PROD    = "\\[prod (?!prod)([^\\[\\]]*)\\]";
const char DELIMITER = '_';
const vector<char> next_vars = { 'b', 'c', 'a' };

// Global vars.
static string op;

static pair<string, string> parse_inequality(const string& inequality)
{
    string LHS;
    string RHS;

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
            cout << "Invalid input... Aborting!" << endl;
            exit(EXIT_FAILURE);
        }
    }

    trim(LHS);
    trim(RHS);

    return pair<string, string>(LHS, RHS);
}

static set<string> get_vars_from_expression(const string& expr)
{
    set<string> result;

    regex words_regex(REGEX_TRIANGLE_VAR);
    auto words_begin = sregex_iterator(expr.begin(), expr.end(), words_regex);
    auto words_end   = sregex_iterator();

    for (sregex_iterator it = words_begin; it != words_end; ++it)
    {
        smatch match = *it;
        string match_str = match.str();
        result.insert(match_str);
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

static void replace_char_with_next_cyclic_char(string& s, int pos, int index)
{
    if (isupper(s[pos]))
    {
        s[pos] = toupper(next_vars.at(index));
    }
    else
    {
        s[pos] = next_vars.at(index);
    }
}

static string get_next_var(const smatch& m)
{
    string var = m.str(0);
    int pos = var.length() - 1;
    char last_char = var[pos];

    if (find(next_vars.begin(),
             next_vars.end(),
             tolower(last_char)) != next_vars.end())
    {
        int index = tolower(last_char) - 'a';
        replace_char_with_next_cyclic_char(var, pos, index);
    }
    else
    {
        // If variable has more than two characters, try to increment (cyclically)
        // the second to last character.
        int pos = var.length() - 2;
        char second_last_char = var[pos];

        if (find(next_vars.begin(),
                 next_vars.end(),
                 tolower(second_last_char)) != next_vars.end())
        {
            int index = tolower(second_last_char) - 'a';
            replace_char_with_next_cyclic_char(var, pos, index);
        }
    }

    return var;
}

static string build_cyclic_expr(const smatch& m)
{
    string e1 = m.str(1);
    string e2 = regex_replace(e1, regex(REGEX_TRIANGLE_VAR), &get_next_var);
    string e3 = regex_replace(e2, regex(REGEX_TRIANGLE_VAR), &get_next_var);

    ostringstream cyclic_sum;
    cyclic_sum << "("
               << "(" << e1 << ") " << op << " "
               << "(" << e2 << ") " << op << " "
               << "(" << e3 << ")"
               << ")";

    return cyclic_sum.str();
}

static void expand_cyclic_sums(string& inequality)
{
    op = "+";
    inequality = regex_replace(inequality, regex(REGEX_CYCLIC_SUM), &build_cyclic_expr);
}

static void expand_cyclic_products(string& inequality)
{
    op = "*";
    inequality = regex_replace(inequality, regex(REGEX_CYCLIC_PROD), &build_cyclic_expr);
}

static std::string replace_vars_with_aliases(const string& expr)
{
    ostringstream result;
    vector<pair<int, string>> matches;

    regex words_regex(REGEX_TRIANGLE_VAR);
    auto words_begin = sregex_iterator(expr.begin(), expr.end(), words_regex);
    auto words_end   = sregex_iterator();

    for (sregex_iterator it = words_begin; it != words_end; ++it)
    {
        smatch match = *it;
        string match_str = match.str();
        matches.push_back({ it->position(), match_str });
    }

    int pos = 0, m_idx = 0;

    for ( ; m_idx < matches.size(); ++m_idx)
    {
        result << expr.substr(pos, matches[m_idx].first - pos);

        // Append either the expression variable or its alias (via map lookup).
        if (elem_alias_map.find(matches[m_idx].second) == elem_alias_map.end())
        {
            result << matches[m_idx].second;
        }
        else
        {
            result << elem_alias_map[matches[m_idx].second];
        }

        // The next position from which we copy characters on the next iteration.
        pos = matches[m_idx].first + matches[m_idx].second.size();
    }

    // Copy the remaining characters (if any).
    result << expr.substr(pos, expr.size() - pos);

    return result.str();
}

#endif /* utils_hpp */

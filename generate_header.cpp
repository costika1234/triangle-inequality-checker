#include <stdio.h>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <regex>
#include <unordered_map>
#include <set>
#include <map>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>

#include "expressions.h"

using namespace std;

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
string op;
unordered_map<string, string> var_to_func = {};

pair<string, string> parse_inequality(string inequality)
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
            exit (EXIT_FAILURE);
        }
    }
    
    boost::trim(LHS); boost::trim(RHS);
    
    return pair<string, string>(LHS, RHS);
}

string convert_to_code(string expr)
{
    // Replace each variable 'var' with 'tr.var'.
    regex var_regex(REGEX_TRIANGLE_VAR);
    return regex_replace(expr, var_regex, "tr.$0");
}

set<string> get_vars_from_expression(string expr)
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

set<string> get_vars_from_inequality(string LHS, string RHS)
{
    set<string> lhs_vars = get_vars_from_expression(LHS);
    set<string> rhs_vars = get_vars_from_expression(RHS);
    
    set<string> all_vars;
    set_union(lhs_vars.begin(),
              lhs_vars.end(),
              rhs_vars.begin(),
              rhs_vars.end(),
              inserter(all_vars, all_vars.begin()));
    
    // for (auto it = all_vars.begin(); it != all_vars.end(); ++it)
    // {
    //    cout << *it << " ";
    // }
    
    return all_vars;
}

void build_var_to_func_map()
{
    ifstream input("Triangle.hpp");
    
    for(string line; getline(input, line);)
    {
        regex re(REGEX_INIT_FUNCTIONS);
        smatch match;
        
        if (regex_search(line, match, re) && match.size() > 1)
        {
            stringstream match_str(match.str(1));
            if (match_str.str() == "Rrs" || match_str.str() == "angles")
            {
                continue;
            }
            
            string var;
            
            while(getline(match_str, var, DELIMITER))
            {
                var_to_func.insert(pair<string, string>(var, "init_" + match_str.str() + "();"));
            }
        }
    }
   
    // Print Map.
    // for (auto it : var_to_func)
    // {
    //    cout << it.first << " --> " << it.second << endl;
    // }
    
    input.close();
}

void generate_LHS_RHS_header(string LHS, string RHS, set<string> vars)
{
    string LHS_expr = convert_to_code(LHS);
    string RHS_expr = convert_to_code(RHS);

    ofstream header_file("inequality.h");
    header_file << "#ifndef LHS_RHS_h"  << endl
                << "#define LHS_RHS_h"  << endl
                << endl
                << "#define LHS_expr (" << LHS_expr << ")" << endl
                << "#define RHS_expr (" << RHS_expr << ")" << endl
                << endl
                << "#define LHS_str \"" << LHS << "\"" << endl
                << "#define RHS_str \"" << RHS << "\"" << endl
                << endl;
    
    ostringstream init_methods;
    set<string> init_methods_set;
    build_var_to_func_map();
    
    for (auto it = vars.begin(); it != vars.end(); ++it)
    {
        if (var_to_func.find(*it) != var_to_func.end())
        {
            init_methods_set.insert(var_to_func[*it]);
        }
    }
    
    for (auto init_method : init_methods_set)
    {
        init_methods << "tr." << init_method << " ";
    }
    
    header_file << "#define INIT_VARS " << init_methods.str() << endl
                << endl
                << "#endif /* LHS_RHS_h */" << endl;
    
    header_file.close();
}

void replace_char_with_next_cyclic_char(string& s, int pos, int index)
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

string get_next_var(const boost::smatch& m)
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

string build_cyclic_expr(const boost::smatch& m)
{
    string e1 = m.str(1);
    string e2 = boost::regex_replace(e1, boost::regex(REGEX_TRIANGLE_VAR), &get_next_var);
    string e3 = boost::regex_replace(e2, boost::regex(REGEX_TRIANGLE_VAR), &get_next_var);

    ostringstream cyclic_sum;
    cyclic_sum << "("
               << "(" << e1 << ") " << op << " "
               << "(" << e2 << ") " << op << " " 
               << "(" << e3 << ")"
               << ")";

    return cyclic_sum.str();
}

string expand_cyclic_sums(string inequality)
{
    op = "+";
    return boost::regex_replace(inequality, 
                                boost::regex(REGEX_CYCLIC_SUM), 
                                &build_cyclic_expr);
}

string expand_cyclic_products(string inequality)
{
    op = "*";
    return boost::regex_replace(inequality, 
                                boost::regex(REGEX_CYCLIC_PROD), 
                                &build_cyclic_expr);
}

int main(int argc, const char * argv[])
{
    ostringstream inequality;
    inequality << "[sum cosA2] >= s / R";

    string inequality_str = inequality.str();
    inequality_str = expand_cyclic_sums(inequality_str);
    inequality_str = expand_cyclic_products(inequality_str);

    auto sides = parse_inequality(inequality_str);
    string LHS = get<0>(sides);
    string RHS = get<1>(sides);
    auto vars = get_vars_from_inequality(LHS, RHS);

    generate_LHS_RHS_header(LHS, RHS, vars);
        
    return 0;
}

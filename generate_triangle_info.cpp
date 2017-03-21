#include <fstream>
#include <string>
#include <sstream>
#include <regex>
#include <stdio.h>
#include <iostream>
#include <boost/algorithm/string/replace.hpp>

using namespace std;

static string INDENT2 = string(8, ' ');
static string INDENT3 = string(12, ' ');
static string MEMBER_FUNC_PTR_KEY = "__POINTERS_TO_MEMBER_FUNCTIONS__";
static string TRIANGLE_ELEM_MAP_KEY = "__MAP_OF_TRIANGLE_ELEMENTS_TO_POINTERS__";
static string REGEX_INIT_FUNCTIONS = "void init_(.*)\\(\\);";
static char DELIMITER = '_';

string get_triangle_elem_key_value(const string& elem,
                                   int           pos,
                                   bool          format=true)
{
    ostringstream triangle_elem_key_value;
    triangle_elem_key_value << "{ \"" << elem << "\", { &tr->" << elem << ", " << pos << " } }";

    if (format)
    {
        triangle_elem_key_value << "," << endl << INDENT3;
    }

    return triangle_elem_key_value.str();
}

pair<string, string> generate_triangle_info()
{
    ostringstream func_ptrs;
    ostringstream member_elems_map;
    ifstream f("triangle.hpp");
    int pos = 0;

    for(string line; getline(f, line);)
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

            func_ptrs << "&Triangle::init_" + match_str.str() + "," << endl << INDENT2;

            string var;
            while(getline(match_str, var, DELIMITER))
            {
                member_elems_map << get_triangle_elem_key_value(var, pos);
            }

            member_elems_map << endl << INDENT3;
            ++pos;
        }
    }

    // Handle special triangle elements which are compute every iteration.
    func_ptrs << "&Triangle::dummy_update_sides";
    vector<string> special_elems = { "a", "b", "c", "R", "r", "s", "S" };
    int idx = 0;

    for ( ; idx < special_elems.size() - 1; ++idx)
    {
        member_elems_map << get_triangle_elem_key_value(special_elems[idx], pos);
    }
    member_elems_map << get_triangle_elem_key_value(special_elems[idx], pos, false);

    return { func_ptrs.str(), member_elems_map.str() };
}

int main(int argc, const char * argv[])
{
    ifstream in("triangle_info_template.txt");
    ofstream out("triangle_info.hpp");

    ostringstream template_stream;
    template_stream << in.rdbuf();
    string triangle_info_cpp = template_stream.str();

    auto triangle_info = generate_triangle_info();

    boost::replace_first(triangle_info_cpp, MEMBER_FUNC_PTR_KEY, triangle_info.first);
    boost::replace_first(triangle_info_cpp, TRIANGLE_ELEM_MAP_KEY, triangle_info.second);

    out << triangle_info_cpp;

    in.close();
    out.close();

    return 0;
}
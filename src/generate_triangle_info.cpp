#include <fstream>
#include <string>
#include <sstream>
#include <regex>
#include <stdio.h>
#include <iostream>

using namespace std;

static string INDENT = string(12, ' ');
static string MEMBER_FUNC_PTR_KEY = "__POINTERS_TO_MEMBER_FUNCTIONS__";
static string TRIANGLE_ELEM_MAP_KEY = "__MAP_OF_TRIANGLE_ELEMENTS_TO_POINTERS__";
static string REGEX_INIT_FUNCTIONS = "void init_(.*)\\(\\);";
static char DELIMITER = '_';

string get_triangle_elem_key_value(const string& elem, int pos)
{
    ostringstream triangle_elem_key_value;

    triangle_elem_key_value << "{ \"" << elem << "\", { &tr->" << elem << ", " << pos << " } }";
    triangle_elem_key_value << "," << endl << INDENT;

    return triangle_elem_key_value.str();
}

pair<string, string> generate_triangle_info()
{
    ostringstream func_ptrs;
    ostringstream member_elems_map;
    ifstream f("include/triangle.hpp");

     // Handle special triangle elements which are computed every iteration.
    func_ptrs << "&Triangle::dummy_update_sides" << "," << endl << INDENT;
    for (auto special_elem : { "a", "b", "c", "A", "B", "C", "R", "r", "s", "S" })
    {
        member_elems_map << get_triangle_elem_key_value(special_elem, 0);
    }
    member_elems_map << endl << INDENT;

    // All the other elements will start from index 1.
    int pos = 1;

    for (string line; getline(f, line);)
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

            func_ptrs << "&Triangle::init_" + match_str.str() + "," << endl << INDENT;

            string var;
            while (getline(match_str, var, DELIMITER))
            {
                member_elems_map << get_triangle_elem_key_value(var, pos);
            }

            member_elems_map << endl << INDENT;

            ++pos;
        }
    }

    // Add dummy elements as the last entries for both maps.
    func_ptrs << "NULL";
    member_elems_map << "{ \"\", { NULL, 0 } }";

    return { func_ptrs.str(), member_elems_map.str() };
}

int main(int argc, const char * argv[])
{
    ifstream in("templates/triangle_info_template.txt");
    ofstream out("include/triangle_info.hpp");

    ostringstream template_stream;
    template_stream << in.rdbuf();
    string triangle_info_cpp = template_stream.str();

    auto triangle_info = generate_triangle_info();

    triangle_info_cpp = regex_replace(triangle_info_cpp,
                                      regex(MEMBER_FUNC_PTR_KEY),
                                      triangle_info.first);
    triangle_info_cpp = regex_replace(triangle_info_cpp,
                                      regex(TRIANGLE_ELEM_MAP_KEY),
                                      triangle_info.second);

    out << triangle_info_cpp;

    in.close();
    out.close();

    return 0;
}

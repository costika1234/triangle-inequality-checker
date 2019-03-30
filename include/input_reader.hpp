#ifndef input_reader_hpp
#define input_reader_hpp

#include "parallel_checker.hpp"

static const tuple<string, vector<string>, vector<string>> read_input()
{
    ifstream f(INPUT_FILE);
    string inequality, line;
    vector<string> constraints, substitutions;

    // The inequality to be checked is located on the first line after comments
    // or blank lines (if present).
    while (getline(f, line) && is_comment_or_empty_line(line));
    inequality = line;

    for (string line; getline(f, line);)
    {
        // Skip any comments or empty lines.
        if (is_comment_or_empty_line(line))
        {
            continue;
        }

        // Disregard anything after '---' (to allow for saving other results
        // to the same file that do not belong to the inequality in question).
        if (is_end_of_input_delimiter(line))
        {
            break;
        }

        if (is_constraint(line))
        {
            constraints.push_back(line);
            continue;
        }

        if (is_substitution(line))
        {
            substitutions.push_back(line);
            continue;
        }

        throw runtime_error("Unknown expression: '" + line + "'!");
    }

    f.close();

    return make_tuple(inequality, constraints, substitutions);
}

#endif /* input_reader_hpp */

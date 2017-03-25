#ifndef print_utils_hpp
#define print_utils_hpp

#define VERY_LOW_PRECISION  5
#define LOW_PRECISION      15
#define HIGH_PRECISION     30

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

void display_LHS_RHS(long_d LHS, long_d RHS)
{
    string ineq_holds_str = (LHS <= RHS) ? "✅" : "❌";
    cout << "(LHS: " << LHS << ", RHS: " << RHS << ") " << ineq_holds_str
         << endl << endl;
}

std::ostream& bold_on(std::ostream& os)
{
    return os << "\e[1m";
}

std::ostream& bold_off(std::ostream& os)
{
    return os << "\e[0m";
}

#endif /* print_utils_hpp */

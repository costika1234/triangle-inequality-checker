#ifndef aliases_hpp
#define aliases_hpp

#include <unordered_map>

using namespace std;

typedef unordered_map<string, string> TrElemAliasMap;

// We need to have this map because the 'exprtk' library does not have support
// for case-sensitive variable names. Also, we need to avoid name clashes with
// built-in functions (e.g. 'sinc')...
static TrElemAliasMap elem_alias_map =
    {
        { "R",    "circumradius"  },
        { "r",    "inradius"      },
        { "s",    "semiperimeter" },
        { "S",    "area"          },
        { "A",    "angleA"        },
        { "B",    "angleB"        },
        { "C",    "angleC"        },
        { "ha",   "heightA"       },
        { "hb",   "heightB"       },
        { "hc",   "heightC"       },
        { "sinC", "sineOfC"       }
    };

#endif /* aliases_hpp */

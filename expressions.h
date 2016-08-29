#ifndef expressions_h
#define expressions_h

#define VERY_LOW_PRECISION  5
#define LOW_PRECISION      15
#define HIGH_PRECISION     30
#define NO_ITERATIONS      20

// Convenient expressions.
#define BLUNDON_LEFT     "2 * R * R + 10 * R * r - r * r - sqrt(4 * R * (R - 2 * r) * (R - 2 * r) * (R - 2 * r))"
#define BLUNDON_RIGHT    "2 * R * R + 10 * R * r - r * r + sqrt(4 * R * (R - 2 * r) * (R - 2 * r) * (R - 2 * r))"
#define WALKER           "2 * R * R + 8 * R * r + 3 * r * r"
#define RIGHT_ANGLED_TR  "2 * R + r"
#define GERRETSEN_LEFT   "16 * R * r - 5 * r * r"
#define GERRETSEN_RIGHT  "4 * R * R + 4 * R * r + 3 * r * r"
#define SUM_SIN_A2_LEFT  "0.5 + sqrt(1 - sqrt(1 - 2 * r / R)) + 0.5 * sqrt(1 - 2 * r / R)"
#define SUM_SIN_A2_RIGHT "0.5 + sqrt(1 + sqrt(1 - 2 * r / R)) - 0.5 * sqrt(1 - 2 * r / R)"
#define SUM_SIN_A2_JG    "((long_d) 4) / ((long_d) 3)  + r / (3 * R)"
#define SUM_HA_LA_BOUND  "1 + sqrt((s * s + r * r + 2 * R * r) / (2 * R * R))"
#define SUM_ADJ_SIDES    "a * b + b * c + c * a"
#define SUM_SIDES_2      "a * a + b * b + c * c"
#define SUM_SIDES_3      "a * a * a + b * b * b + c * c * c"
#define SUM_SIDES_4      "a * a * a * a + b * b * b * b + c * c * c * c"
#define t                "R / r"
#define IRAN_96          "(a * b + b * c + c * a) * (1 / ((a + b) * (a + b)) + 1 / ((b + c) * (b + c)) + 1 / ((c + a) * (c + a)))"

#endif /* expressions_h */

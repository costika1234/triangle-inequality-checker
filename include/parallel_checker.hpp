#ifndef parallel_hpp
#define parallel_hpp

#include <future>

#include "checker.hpp"

#define MAX_T_THRESHOLD 20000

class ParallelChecker
{
private:
    string inequality;
    string inequality_LHS, inequality_RHS;

    vector<string> constraints;
    vector<string> constraints_LHS, constraints_RHS;

    // Number of constraints.
    int no_constraints;

    // min_angle <= min{A, B, C}.
    long_d min_angle;

    // max{A, B, C} <= max_angle.
    long_d max_angle;

    // max{A, B, C} >= phi_angle.
    long_d phi_angle;

    // Iteration step.
    long_d step;

    // Additional flag for 'display_stats'.
    const bool display_min_max_triangles = false;

    // Balanced partitions for ranges.
    const vector<vector<long_d>> partitions =
        {
            {   0,  500,   0,  280 },
            {   0,  500, 280, 1000 },
            { 500, 1000,   0,  740 },
            { 500, 1000, 740, 1000 }
        };

    const int no_partitions = partitions.size();

    // A vector of 'no_partitions' pointers to 'Checker' instances.
    vector<Checker *> checkers;

    // A vector of 'no_partitions' futures.
    vector<future<void>> futures;

    // The final aggregated stats.
    TriangleStats finalStats;

    void aggregate_stats();

public:
    ParallelChecker(string inequality,
                    vector<string> constraints,
                    long_d min_angle = 0.0,
                    long_d max_angle = 180.0,
                    long_d phi_angle = 0.0,
                    long_d step      = 0.1);

    void run();

    TriangleStats get_stats();

    void display_stats();
};

#endif /* parallel_hpp */
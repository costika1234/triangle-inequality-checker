#include "parallel_checker.hpp"

using namespace std;

const string read_input(const string& filename)
{
    string inequality;
    ifstream f(filename);
    getline(f, inequality);
    f.close();

    return inequality;
}

int main(int argc, const char * argv[])
{
    if (argc != 1 && argc != 4)
    {
        return 1;
    }

    long_d min_angle = 0.0;
    long_d max_angle = 180.0;
    long_d phi_angle = 0.0;
    
    if (argc == 4)
    {
        min_angle = atof(argv[1]);
        max_angle = atof(argv[2]); 
        phi_angle = atof(argv[3]);
    }

    ParallelChecker parallel_checker(read_input("inequality.txt"), min_angle, max_angle, phi_angle);

    parallel_checker.run();
    parallel_checker.display_stats();

    return 0;
}

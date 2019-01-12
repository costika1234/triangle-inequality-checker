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
        // For convenience, angles are expressed in degrees on the command line.
        min_angle = convert_degrees_to_radians(atof(argv[1]));
        max_angle = convert_degrees_to_radians(atof(argv[2]));
        phi_angle = convert_degrees_to_radians(atof(argv[3]));
    }

    try
    {
        ParallelChecker parallel_checker(read_input("inequality.txt"), 
                                         min_angle, 
                                         max_angle, 
                                         phi_angle);
        parallel_checker.run();
        parallel_checker.display_stats();
    }
    catch (const runtime_error& error)
    {
        cout << "ERROR! " << error.what() << endl;
    }

    return 0;
}

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
    ParallelChecker parallel_checker(read_input("inequality.txt"));
    parallel_checker.run();
    parallel_checker.display_stats();

    return 0;
}

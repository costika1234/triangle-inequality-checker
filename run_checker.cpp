#include "checker.hpp"

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
    Checker checker(read_input("inequality.txt"));
    checker.run();

    return 0;
}

#ifndef utils_h
#define utils_h

#include <stdio.h>
#include <cmath>

typedef long double long_d;

using namespace std;

static bool almost_equal_relative(long_d A,
                                  long_d B,
                                  float maxRelDiff = FLT_EPSILON)
{
    // Calculate the difference.
    long_d diff = fabs(A - B);
    A = fabs(A);
    B = fabs(B);
    
    // Find the largest
    long_d largest = (B > A) ? B : A;
    
    return (diff <= largest * maxRelDiff);
}

#endif /* utils_h */
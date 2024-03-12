#include "scalc2.h"

int lcm(int a, int b) {
    return a*b/gcd(a, b);
}

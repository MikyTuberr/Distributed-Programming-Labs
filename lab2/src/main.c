#include <stdlib.h>
#include <stdio.h>
#include "scalc1.h"
#include "scalc2.h"
#include "dcalc1.h"
#include "dcalc2.h"

int main() {
    char* red = "\033[0;31m";
    char* blue = "\033[0;34m";
    char* purple = "\033[0;35m";
    char* cyan = "\033[0;36m";
    char* white = "\033[0;37m";
    printf("\n%sFactorial of 30 = %d\n", red,factorial(30));
    printf("%s5th fibonacci number = %d\n",blue,fib(5));
    printf("%sLeast Common Multiple of 20 and 5 = %d\n",purple,lcm(20, 5));
    printf("%sGreatest Common Divisor of 120 and 50 = %d\n\n",cyan, gcd(120, 50));
    return 0;
}

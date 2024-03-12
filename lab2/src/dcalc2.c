#include "dcalc2.h"

int fib(int a) {
    if(a == 1 || a == 0) {
        return 1;
    }
    return fib(a-1)+fib(a-2);
}

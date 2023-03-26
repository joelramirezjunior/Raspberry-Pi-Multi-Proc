#include "rpi.h"
#define UPPERBOUND 10

int recurse(int x) {
    if( x == 0) return 0;
    return recurse(--x);
}

void notmain(void) {
    recurse(UPPERBOUND);
}
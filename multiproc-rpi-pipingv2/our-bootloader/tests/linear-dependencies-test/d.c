#include "rpi.h"

int do_stuff(int a) {
    unsigned long long i = 0;
    printk("     doing stuff... should take a bit\n");
    while (0 != i++) {
    	a++;
    }
    return 140 * a + 0xe;
}

void notmain(void) {
    printk("continuing linear dependencies test.\n");
    printk("starting d, which is depended on by NOTHING. ----------------------\n");
    //todo: int d = READ FROM PIPE;
    int d = 10;
    printk("from c, got output: ---%d---\n", d);
    trace("from c, got output: ---%d---\n", d);
    printk("done with d. --------------------------------------\n");
    printk("done with all linear dependencies. you should see the correct value above, showing everything worked.\n");
}


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
    printk("starting c, which is depended on by d. ----------------------\n");
    //todo: int c = READ FROM PIPE;
    int c = 10;
    printk("doing stuff with starting number of: ---%d---, writing the output so that d can read it\n", c);
    c = do_stuff(c);
    trace("%d", c);
    printk("wrote output of ---%d--- which d will expect\n", c);
    printk("done with c. moving on... --------------------------------------\n");
}


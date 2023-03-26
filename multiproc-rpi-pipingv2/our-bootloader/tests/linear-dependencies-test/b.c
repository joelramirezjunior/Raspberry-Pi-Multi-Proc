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
    printk("starting b, which is depended on by c. ----------------------\n");
    //todo: int b = READ FROM PIPE;
    int b = 10;
    printk("doing stuff with starting number of: ---%d---, writing the output so that c can read it\n", b);
    b = do_stuff(b);
    trace("%d", b);
    printk("wrote output of ---%d--- which c will expect\n", b);
    printk("done with b. moving on... --------------------------------------\n");
}


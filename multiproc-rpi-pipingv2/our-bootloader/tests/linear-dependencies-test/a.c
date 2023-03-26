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
    printk("starting linear dependencies test.\n");
    printk("starting on a, which is depended on by b. ----------------------\n");
    int a = 7;
    a = do_stuff(a);
    printk("did stuff with starting number of: ---7---, writing the output so that b can read it\n");
    trace("%d\n", a); 
    printk("wrote output of ---%d--- which b will expect\n", a);
    printk("done with a. moving on... --------------------------------------\n");
}


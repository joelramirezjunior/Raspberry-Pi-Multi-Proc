#include "rpi.h"
#include "memmap.h"

void notmain(void) {
    unsigned a = 0xFFFFFFFF;
    unsigned b = 0xFFFFFFFF;
    char *c = "POSDFKPSDOFKDS\n";
    char *d = "POSDFKPSDOFKDS\n";
    char *e = "POSDFKPSDOFKDS\n";
    char *f = "POSDFKPSDOFKDS\n";
    for (int i = 0; i < 10000; i++){ 
	c += 1;
	d += 2;
	e += 3;
	f += 4;
    }
    printk("%s\n", c);
    printk("%s\n", d);
    printk("%s\n", e);
    printk("%s\n", f);
    printk("%x\n", &a);
    printk("%x\n", &b);
    printk("%x\n", __data_start__);
    printk("%x\n", __data_end__);
}


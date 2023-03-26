#include <stdio.h>
#include "rpi.h"
#include "memmap.h"
void notmain(void) { 
    printk("In the c.bin program. We will be seeing if we can access\nthe dependencies' data from the __data_start__ variable\n");
    printk("currently, data start is %x\n", __data_start__);
    unsigned data_n = __data_start__[0];
    printk("The dependencies' outputs received have %d bytes total\n", data_n);
    printk("attempt 2: %d\n", *((uint32_t*)__data_start__));
    printk("Here is the dependency files' outputs: ------------------\n");
    int offset = sizeof(unsigned);
    for (int i = 0; i < 100; i++) {
	printk("%c", (char *)(__data_start__ + i));
	//offset += parentsizes[i];
    }
    printk("[END]----------------------------------------------------\n");

}

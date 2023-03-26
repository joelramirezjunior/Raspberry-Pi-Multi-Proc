## uncomment COMMON_SRC and comment out STAFF_OBJS
## COMMON_SRC += mbr.c
#STAFF_OBJS +=  staff-mbr.o
#
## uncomment COMMON_SRC and comment out STAFF_OBJS
## COMMON_SRC += fat32.c
#STAFF_OBJS +=  staff-fat32.o
#
## the tests in decreasing order of difficulty.
#PROGS := $(wildcard ./test-basic/*.c)
#
#CFLAGS_EXTRA  = -Iexternal-code
#
## a list of all of your object files.
#COMMON_SRC := a.c b.c c.c hello.c
#
## external-code/bzt-sd.c 
#
#TTYUSB = 
#BOOTLOADER = my-install
#RUN = 1
#
#EXCLUDE ?= grep -v simple_boot
#GREP_STR := 'HASH:\|ERROR:\|PANIC:\|PASS:\|TEST:'
#include $(CS140E_2023_PATH)/libpi/mk/Makefile.template-v2
#
#clean::
#	rm -f tests/*.list tests/*.o tests/*.elf tests/*~ tests/*.bin
#	make -C external-code clean

# ^ above is defunct 



PROGS := $(wildcard ./test-basic/*.c) 
#PROGS := $(wildcard ./linear-dependencies-test/*.c)
#PROGS += $(wildcard ./multiple-dependencies-test/*.c)
#PROGS += $(wildcard ./forked-dependants-test/*.c)
PROGS := $(wildcard ./length-dependancy-test/*.c)
PROGS := $(wildcard ./lengthy-depedancy-test/*.c)


# shared code linked with all driver programs.

# define this if you need to give the device for your pi
TTYUSB = 
BOOTLOADER = pi-install

GREP_STR ?= 'TRACE:\|ERROR:\|PANIC:'
BOOT_CMD := $(BOOTLOADER) $(TTYUSB)

# uncomment if you want ita to automatically run.
RUN=0

#MYTEST_SRC := linear-dependencies-test/ multiple-dependencies-test/ forke-dependants-test/
#MYTESTS := $(patsubst %.c, %, $(TEST_SRC))
#MYTEST_PIPE := $(patsubst %.bin, %.test, $(TESTS))
#
#pipe: $(MYTEST_PIPE)
#
##this whole thing is super hardcoded, beware
#%.test: %.bin  
#	
#	@make clean
#	@echo -n "checking <$*>: "
#	@make emit 
#	@echo "Here is the output that any possible dependents will read"
#	@/bin/cat linear-dependencies-test/$*.out
#	@/bin/cat linear-dependencies-test/$*.out | grep  $(GREP_STR) | cut -c15- > linear-dependencies-test/$*.test
##	@echo < /bin/cat linear-dependencies-test/$*.out | grep $(GREP_STR) > linear-dependencies-test/$*.test

#figure out how to un hard code this lmfao
#maybe don't make it convolulted: right now its emit -> out -> output, perhaps just emit -> output
#pipe:
#	
#	@make clean
#	@echo -n "checking <a>: "
#	@make emit 
#	@echo "Here is the output that any possible dependents will read"
#	@/bin/cat linear-dependencies-test/a.out
#	@/bin/cat linear-dependencies-test/a.out | grep  $(GREP_STR) | cut -c15- > linear-dependencies-test/a.output

include $(CS140E_FINALPROJ_PATH)/libpi/mk/Makefile.template

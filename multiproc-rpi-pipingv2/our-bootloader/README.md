How our programs pipe input and output to each other:

---

first, overview of how programs running on the pi can output to our terminal:
- in the unix side bootloader, after the program is sent over, the code calls pi-echo.c, which terminates execution.
	- in pi-echo, essentially what happens is the unix side reads in from a file descriptor (fd 3 probably) the stuff that the pi outputted, which is things like printk and also the DONE!!! message. 
	- these reads (read to buffer then print, in a loop) show up on our terminal
	- once the reading times out we exit the loop with a message about the exit

---

how "piping" works between our pis running programs that are dependent on each other:

- have the dependent program output its dependency output using pi output (done) to a .output file sharing the same root name, by putting a read and write in pi-echo.c so that after the bootloading sequence and execution of the pi program we can save this output.
- myprogram then will continue on and when it runs a bin that has dependencies, it will read and pass in all of those dependencies' outputs, which are in files corresponding to the dependent's names (eg b depends on a, and a's output is stored in a.output) 
- todo: figure out how to pass in the dependencies' outputs so that the bin can see them (write to sd? or literally edit the .c of the bin to contain a char[] with the input then recompile before sending? sounds janky) (this is the biggest question)
- Currently two ideas: 
	- first one is to modify the bootloader, so that it also accepts some data as well as code, (the data being the dependencies' info), and the bootloader writes that data (which hopefully fits somehwere) to some spot in memory. Then, upon branching in main.c of 2-pi-side, we can access that memory again. Main things to figure out: how do I decide the address, make sure nothing is overlapping, and then save that address for use after branching? And, is there anything special to consider when modifying the bootloading sequence? Should I just add it to the buf that simple-boot sends to get_code?? 
	- second idea is ugly: write to a blank array in the dependents' file (literally open the .c file and write the output stuff as a string somewhere) then in the course of the program, before bootloading, use shell commands in the c program (or something like that) to re-make into a bin. janky but maybe it would work. 



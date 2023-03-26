#include "libunix.h"
#include "simple-boot.h"

// Note: if timeout in <set_tty_to_8n1> is too small (set by our caller)
// you can fail here. when you do a read and the pi doesn't send data quickly enough.
void simple_boot(int fd, const uint8_t *buf, unsigned n, const uint8_t *data_buf, unsigned data_n, unsigned n_payloads) { 
    // all implementations should have the same message: same bytes,
    // same crc32: cross-check these values to detect if your <read_file> 
    // is busted.
    trace("simple_boot: sending %d bytes, crc32=%x\n", n, crc32(buf,n));

    boot_output("waiting for a start\n");
    uint32_t op;




    // step 0: drain the initial data.  can have garbage.  
    // 
    // the code is a bit odd b/c 
    // if we have a single garbage byte, then reading 32-bits will
    // will not match <GET_PROG_INFO> (obviously) and if we keep reading 
    // 32 bits then we will never sync up with the input stream, our hack
    // is to read a byte (to try to sync up) and then go back to reading 32-bit
    // ops.
    //
    // CRUCIAL: make sure you use <get_op> for the first word in each 
    // message.
    while((op = get_op(fd)) != GET_PROG_INFO) {
        output("expected initial GET_PROG_INFO, got <%x>: discarding.\n", op);
        // have to remove just one byte since if not aligned, stays not aligned
        get_uint8(fd);
    } 

    // 1. reply to the GET_PROG_INFO
    //todo("reply to GET_PROG_INFO");
    trace_put32(fd, PUT_PROG_INFO);
    trace_put32(fd, ARMBASE);
    trace_put32(fd, n);
    trace_put32(fd, crc32(buf, n));
    //send data metadata over now
    trace_put32(fd, data_n);
    trace_put32(fd, n_payloads);
    trace_put32(fd, crc32(data_buf, data_n));

    // 2. drain any extra GET_PROG_INFOS
    //todo("drain any extra GET_PROG_INFOS");
    while((op = get_op(fd)) == GET_PROG_INFO) {
        //output("expected initial GET_PROG_INFO, got <%x>: discarding.\n", op);
        // have to remove just one byte since if not aligned, stays not aligned
	output("draining extra get prog infos, step 1\n");
    } 

    // 3. check that we received a GET_CODE
    //todo("check that we received a GET_CODE");
    //if (op != GET_CODE || (op = get_op(fd)) != crc32(buf, n)) 
//	    panic("op is not get code in step 3");
    while (op != GET_CODE) {
    	output("Expected GET_CODE\n");
	op = get_op(fd);
    }

    // 4. handle it: send a PUT_CODE + the code.
    //todo("send PUT_CODE + the code in <buf>");
    if ((op = get_op(fd)) == crc32(buf, n)) {
    	trace_put32(fd, PUT_CODE);
	for (int i = 0; i < n; i++) {
       	    trace_put8(fd, buf[i]);
	}
    }

    //FINAL PROJ: only do if values aren't 0 since if they are = no dependencies
    //^see get-code.c code for more, in 2-pi-side
    if (data_n != 0 && n_payloads != 0) {
    	//final proj: check for a GET_DATA
    	while (op != GET_DATA) {
    	    output("Expected GET_DATA\n");
    	    op = get_op(fd);
    	}
    	//final proj: handle it: send a PUT_DATA + the data
    	if ((op = get_op(fd)) == crc32(data_buf, data_n)) {
    	    trace_put32(fd, PUT_DATA);
    	    for (int i = 0; i < data_n; i++) {
    	        trace_put8(fd, data_buf[i]);
    	    }
    	}
    }

    // 5. Wait for BOOT_SUCCESS
    ck_eq32(fd, "BOOT_SUCCESS mismatch", BOOT_SUCCESS, get_op(fd));
    boot_output("bootloader: Done.\n");
}

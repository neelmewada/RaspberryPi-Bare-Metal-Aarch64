/* 
*  Copyright (C) 2021 Neel Mewada - All Rights Reserved
*/


#include "common.h"
#include "utils.h"
#include "io/uart.h"
#include "graphics/framebuffer.h"
#include "drivers/mbox.h"
#include "irq.h"
#include "printf.h"
//#include "fork.h"
//#include "sched.h"
//#include "drivers/vc4/v3d.h" // Low Level driver
//#include "graphics/v3d.h" // High Level API

void process(char *array) {
    while (1) {
        for (u8 i = 0; i < 5; i++) {
            uart_send(array[i]);
            delay(100000);
        }
    }
}

void putc(void *p, char c) {
    if (c == '\n') {
        uart_send('\r');
    }
    uart_send(c);
}

void kernel_main() {
    uart_init();
    uart_puts("UART Done!\n");
    init_printf(0, putc);

    printf("\nRaspberry PI Bare Metal OS initializing...\n");

    irq_init_vectors();
    enable_interrupt_controller();

#ifdef TARGET_RPI3
    printf("\nTarget Platform: Raspberry PI 3\r\n");
#endif
#ifdef TARGET_QEMU
    printf("\nTarget Platform: QEMU\r\n");
#endif
    
    u64 board_serial;

    mbox_property_init();
    u32 i = mbox_property_tag(MBOX_TAG_GETBOARDSERIAL, 2, 0, 0);
    mbox_property_end();

    if (mbox_property_exec()) {
        printf("Board serial succeeded!\n");
        board_serial = ((u64)get_mbox(i+1) << 32) | ((u64)get_mbox(i));
    } else {
        printf("Board serial failed!\n");
        board_serial = 0;
    }

    printf("Board Serial is: %X\n", board_serial);

    fb_init(1280, 720);

    // display an ASCII string on screen with PSF
    fb_print(0, 0, "Hello World! This is HDMI Output from Raspberry PI");

    irq_enable();

    u32 clk_rate_final = mbox_set_clock_rate(MBOX_CLOCK_ID_V3D, 250*1000*1000, 0); // set v3d clock 250 MHz
    u8 qpu_status = mbox_set_qpu(1); // enable qpu
    
    int res = copy_process((u64)&process, (u64)"12345");
    if (res != 0) {
        printf("Error while starting process 1\n");
        return;
    }

    res = copy_process((u64)&process, (u64)"abcde");
    if (res != 0) {
        printf("Error while starting process 2\n");
        return;
    }

    while(1) {
        // infinite loop
        //schedule();
    }
}

void handle_uart_recv(char c) {
    uart_puts("UART Recv: ");
    uart_send(c);
    uart_puts("\n");
}


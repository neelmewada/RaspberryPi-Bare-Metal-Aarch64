#include "peripherals/irq.h"
#include "peripherals/aux.h"
#include "irq.h"
#include "utils.h"
#include "io/gpio.h"
#include "io/uart.h"
#include "printf.h"

const char *entry_error_messages[] = {
	"SYNC_INVALID_EL1t",
	"IRQ_INVALID_EL1t",		
	"FIQ_INVALID_EL1t",		
	"ERROR_INVALID_EL1T",		

	"SYNC_INVALID_EL1h",		
	"IRQ_INVALID_EL1h",		
	"FIQ_INVALID_EL1h",		
	"ERROR_INVALID_EL1h",		

	"SYNC_INVALID_EL0_64",		
	"IRQ_INVALID_EL0_64",		
	"FIQ_INVALID_EL0_64",		
	"ERROR_INVALID_EL0_64",	

	"SYNC_INVALID_EL0_32",		
	"IRQ_INVALID_EL0_32",		
	"FIQ_INVALID_EL0_32",		
	"ERROR_INVALID_EL0_32"	
};

void show_invalid_entry_message(u32 type, u64 esr, u64 address) {
    uart_puts("ERROR CAUGHT! ESR:  0x");
    uart_hex((u32)(esr >> 32));
    uart_hex((u32)esr);
    uart_puts("\n");

    uart_puts("Error Type: ");
    uart_puts(entry_error_messages[type]);
    uart_puts("\n");
    printf("%s - %d, ESR: %X, Address: %X\r\n",
        entry_error_messages[type], type, esr, address);
}

void enable_interrupt_controller() {
    REGS_IRQ->irq_enable_1 = AUX_IRQ | SYSTIMER_IRQ_1;
}

void handle_irq() {
    u32 irq = REGS_IRQ->irq_pending_1;
	
	if (irq & AUX_IRQ) { // if it's AUX interrupt
		irq &= ~AUX_IRQ;
		if ((REGS_AUX->mu_iir & 4) == 4) { // if UART received
			//uart_send(uart_recv());
			handle_uart_recv(uart_recv());
		}
	} else if (irq & SYSTIMER_IRQ_1) { // if it's a timer interrupt
        printf("Timer called!\n");
        irq &= ~SYSTIMER_IRQ_1;
        handle_timer_irq();
    }
}


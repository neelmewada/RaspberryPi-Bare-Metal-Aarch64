#include "common.h"
#include "io/gpio.h"
#include "utils.h"
#include "peripherals/aux.h"
#include "io/uart.h"

#include <stdint.h>
#include <limits.h>

#define TXD 14
#define RXD 15


void uart_init()
{
    // NEW CODE
    gpio_pin_set_func(TXD, GFAltFunc5);
    gpio_pin_set_func(RXD, GFAltFunc5);

    gpio_pin_enable(TXD);
    gpio_pin_enable(RXD);

    REGS_AUX->enables = 1;
    REGS_AUX->mu_control = 0;
    REGS_AUX->mu_ier = 1; // interrupt when receive is NOT empty
    REGS_AUX->mu_lcr = 3;
    REGS_AUX->mu_mcr = 0;
    REGS_AUX->mu_baud_rate = 433;

    REGS_AUX->mu_control = 3; // enable Tx, Rx

    uart_puts("\nUART Initialized!\n");
}

void uart_send(char c) {
    while (!(REGS_AUX->mu_lsr & 0x20));
    
    REGS_AUX->mu_io = c;
}

char uart_recv() {
    while (!(REGS_AUX->mu_lsr & 0x01));

    char r = (char)(REGS_AUX->mu_io & 0xFF);
    return r == '\r' ? '\n' : r;  // convert carrige return to newline
}

bool uart_avail() {
    return (REGS_AUX->mu_lsr & 0x01);
}

void uart_puts(char *s) {
    while(*s) {
        /* convert newline to carrige return + newline */
        if(*s=='\n')
            uart_send('\r');
        uart_send(*s++);
    }
}

/**
 * Display a binary value in hexadecimal
 */
void uart_hex(u32 d) {
    u32 n;
    int c;
    for(c = 28; c >= 0; c -= 4) {
        // get highest tetrad
        n = (d >> c) & 0xF;
        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        n += n > 9 ? 0x37 : 0x30;
        uart_send((char)n);
    }
}

void uart_hex_f(float f) {
    u32 n;
    int c;
    
    for(c = 28; c >= 0; c -= 4) {
        // get highest tetrad
        n = ((u32)f >> c) & 0xF;
        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        n += n > 9 ? 0x37 : 0x30;
        uart_send((char)n);
    }
}

unsigned int countSetBitsChar(char n);

unsigned int countSetBitsChar(char n) 
{ 
    unsigned int count = 0; 
    while (n) 
    { 
      n &= (n-1); 
      count++; 
    } 
    return count; 
}

unsigned int countSetBitsFloat(float x);

unsigned int countSetBitsFloat(float x) 
{ 
    // Count number of chars (or bytes) in binary representation of float 
    unsigned int n = sizeof(float)/sizeof(char); 
  
    // typcast address of x to a char pointer 
    char *ptr = (char *)&x;
  
    int count = 0;   // To store the result 
    for (int i = 0; i < n; i++) 
    { 
         count += countSetBitsChar(*ptr); 
         ptr++; 
    }
    return count;
} 

void uart_float_bits(float f) {
    uart_puts("Float has ");
    uart_send(countSetBitsFloat(f));
    uart_puts(" total set bits\n");
}

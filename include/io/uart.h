#pragma once

#include "common.h"

void uart_init();
void uart_send(char c);
char uart_recv();
void uart_puts(char *s);
void uart_hex(u32 d);
void uart_hex_f(float f);
void uart_float_bits(float f);

bool uart_avail();

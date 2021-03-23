#pragma once

#include "common.h"

void irq_init_vectors();
void irq_enable();
void irq_disable();

void enable_interrupt_controller();

void handle_uart_recv(char c);
void handle_timer_irq();

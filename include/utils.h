#pragma once

#include "common.h"

extern void delay(u64 ticks);
void delay_micros(u32 ms);

extern void put32(u64 address, u32 value);
extern u32 get32(u64 address);

extern u32 get_el();
extern u64 get_cntfrq();
extern u64 get_cntpct();

u64 get_system_timer();


#include "common.h"
#include "peripherals/base.h"
#include "utils.h"

#define SYSTMR_LO        ((volatile u32*)(MMIO_BASE+0x00003004))
#define SYSTMR_HI        ((volatile u32*)(MMIO_BASE+0x00003008))


u64 get_system_timer()
{
    u32 h=-1, l;
    // we must read MMIO area as two separate 32 bit reads
    h=*SYSTMR_HI;
    l=*SYSTMR_LO;
    // we have to repeat it if high word changed during read
    if(h!=*SYSTMR_HI) {
        h=*SYSTMR_HI;
        l=*SYSTMR_LO;
    }
    // compose long int value
    return ((u64)h << 32) | l;
}

/**
 * Wait N microsec (with BCM System Timer)
 */
void delay_micros(u32 ms)
{
    u64 t = get_system_timer();
    // we must check if it's non-zero, because qemu does not emulate
    // system timer, and returning constant zero would mean infinite loop
    if(t) while(get_system_timer() < t + ms);
}


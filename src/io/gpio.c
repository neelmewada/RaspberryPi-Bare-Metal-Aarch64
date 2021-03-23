#include "common.h"
#include "io/gpio.h"
#include "peripherals/gpio.h"
#include "utils.h"

void gpio_pin_set_func (u8 pinNumber, GpioFunc func) {
    u8 bitStart = (pinNumber % 10) * 3;
    //u8 bitStart = (pinNumber * 3) % 30;
    u8 func_reg = pinNumber / 10;

    u32 selector = REGS_GPIO->func_select[func_reg];
    selector &= ~(7 << bitStart);   // Clear all the 3 FuncSelect bits
    selector |= (func << bitStart); // Set the FuncSelect bits value

    REGS_GPIO->func_select[func_reg] = selector;
}

void gpio_pin_enable (u8 pinNumber) {
    REGS_GPIO->pupd_enable = 0;
    delay(150);
    REGS_GPIO->pupd_enable_clocks[pinNumber / 32] = 1 << (pinNumber % 32);
    delay(150);
    REGS_GPIO->pupd_enable = 0;
    REGS_GPIO->pupd_enable_clocks[pinNumber / 32] = 0;
}



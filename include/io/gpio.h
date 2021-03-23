#pragma once


typedef enum _GpioFunc {
    GFInput = 0,
    GFOutput = 1,
    GFAltFunc0 = 4,
    GFAltFunc1 = 5,
    GFAltFunc2 = 6,
    GFAltFunc3 = 7,
    GFAltFunc4 = 3,
    GFAltFunc5 = 2,
} GpioFunc;

void gpio_pin_set_func (u8 pinNumber, GpioFunc func);

void gpio_pin_enable (u8 pinNumber);

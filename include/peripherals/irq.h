#pragma once

#include "common.h"
#include "peripherals/base.h"

enum vc_irqs {
    AUX_IRQ = (1 << 29),
    SYSTIMER_IRQ_1 = (1 << 1),
    SYSTIMER_IRQ_3 = (1 << 3)
};

struct arm_irq_regs {
    reg32 irq_pending_0;
    reg32 irq_pending_1;
    reg32 irq_pending_2;
    reg32 fiq_control;
    reg32 irq_enable_1;
    reg32 irq_enable_2;
    reg32 irq_enable_0;
    reg32 res1;
    reg32 irq_disable_1;
    reg32 irq_disable_2;
    reg32 irq_disable_0;
};

#define REGS_IRQ ((struct arm_irq_regs *)(MMIO_BASE + 0x0000B200))




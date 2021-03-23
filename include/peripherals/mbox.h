#pragma once

#include "common.h"
#include "peripherals/base.h"

// a properly aligned buffer
extern volatile u32 mbox[128];

int mbox_call(u8 channel);

#define MBOX_REQUEST    0


#define MBOX_BASE       (MMIO_BASE + 0x0000B880)
#define MBOX_READ       ((reg32*)(MBOX_BASE+0x0))
#define MBOX_POLL       ((reg32*)(MBOX_BASE+0x10))
#define MBOX_SENDER     ((reg32*)(MBOX_BASE+0x14))
#define MBOX_STATUS     ((reg32*)(MBOX_BASE+0x18))
#define MBOX_CONFIG     ((reg32*)(MBOX_BASE+0x1C))
#define MBOX_WRITE      ((reg32*)(MBOX_BASE+0x20))


#define MBOX_RESPONSE   0x80000000
#define MBOX_FULL       0x80000000
#define MBOX_EMPTY      0x40000000

// channels
#define MBOX_CH_POWER   0
#define MBOX_CH_FB      1
#define MBOX_CH_VUART   2
#define MBOX_CH_VCHIQ   3
#define MBOX_CH_LEDS    4
#define MBOX_CH_BTNS    5
#define MBOX_CH_TOUCH   6
#define MBOX_CH_COUNT   7
#define MBOX_CH_PROP    8



#pragma once

#include "common.h"

typedef u32 mbox_tag;

// Mbox tags
#define MBOX_TAG_LAST               0
#define MBOX_TAG_GETBOARDSERIAL     0x10004
#define MBOX_TAG_GET_CLKRATE        0x30002
#define MBOX_TAG_SET_CLKRATE        0x38002

// GPU Tags
#define MBOX_TAG_GPU_ALLOC_MEMORY       0x3000c
#define MBOX_TAG_GPU_LOCK_MEMORY        0x3000d
#define MBOX_TAG_GPU_UNLOCK_MEMORY      0x3000e
#define MBOX_TAG_GPU_RELEASE_MEMORY     0x3000f
#define MBOX_TAG_GPU_EXEC_CODE          0x30010
#define MBOX_TAG_GPU_EXEC_QPU_CODE      0x30011
#define MBOX_TAG_GPU_SET_ENABLEQPU      0x30012


// Mbox Framebuffer tags
#define MBOX_TAG_FB_ALLOCATE            0x40001
#define MBOX_TAG_FB_RELEASE             0x48001
#define MBOX_TAG_FB_BLANKSCREEN         0x40002
#define MBOX_TAG_FB_GET_PHYS_WH         0x40003
#define MBOX_TAG_FB_TEST_PHYS_WH        0x44003
#define MBOX_TAG_FB_SET_PHYS_WH         0x48003
#define MBOX_TAG_FB_GET_VIRT_WH         0x40004
#define MBOX_TAG_FB_TEST_VIRT_WH        0x44004
#define MBOX_TAG_FB_SET_VIRT_WH         0x48004
#define MBOX_TAG_FB_GET_DEPTH           0x40005
#define MBOX_TAG_FB_TEST_DEPTH          0x44005
#define MBOX_TAG_FB_SET_DEPTH           0x48005
#define MBOX_TAG_FB_GET_PX_ORDER        0x40006
#define MBOX_TAG_FB_TEST_PX_ORDER       0x44006
#define MBOX_TAG_FB_SET_PX_ORDER        0x48006
#define MBOX_TAG_FB_GET_PITCH           0x40008
#define MBOX_TAG_FB_GET_VIRT_OFFSET     0x40009
#define MBOX_TAG_FB_TEST_VIRT_OFFSET    0x44009
#define MBOX_TAG_FB_SET_VIRT_OFFSET     0x48009
#define MBOX_TAG_FB_GET_OVERSCAN        0x4000A
#define MBOX_TAG_FB_TEST_OVERSCAN       0x4400A
#define MBOX_TAG_FB_SET_OVERSCAN        0x4800A
#define MBOX_TAG_FB_GET_PALETTE         0x4000B
#define MBOX_TAG_FB_TEST_PALETTE        0x4400B
#define MBOX_TAG_FB_SET_PALETTE         0x4800B



// Clock IDs for tags
#define MBOX_CLOCK_ID_UART   2
#define MBOX_CLOCK_ID_ARM    3
#define MBOX_CLOCK_ID_CORE   4
#define MBOX_CLOCK_ID_V3D    5
#define MBOX_CLOCK_ID_H264   6


// Main functions

void mbox_property_init();
u32 mbox_property_tag(mbox_tag tag, u32 value_size_words, ...);
void mbox_property_end();

int mbox_property_exec();
void mbox_property_discard();

u8 mbox_is_processing();

u32 mbox_start();
u32 mbox_tags(u32 index, u32 tag_id, u32 value_size_words, u32 values[]);
u32 mbox_end(u32 index);
void mbox_size(u32 count);

u32 get_mbox(u8 index);
void set_mbox(u8 index, u32 value);


//---------------------------------------------------------
// Simple mbox functions

/*
* Sets the QPU to enable/disable. Returns 0 if failed.
*/
u8 mbox_set_qpu(u32 state);

/*
* Sets the QPU to enable/disable. Returns 0 if failed.
*/
u32 mbox_set_clock_rate(u32 clock_id, u32 rate_hz, u32 skip_turbo);


//---------------------------------------------------------
// Framebuffer mbox functions

void mbox_framebuffer_init();


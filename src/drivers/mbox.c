#include "peripherals/mbox.h"
#include "drivers/mbox.h"
#include <stdarg.h>
#include <string.h>

volatile u32 __attribute__((aligned(16))) mbox[128];
static u32 _index;
static u8 _is_in_use = 0;


int mbox_call(u8 channel) {
    u32 r = (((u32)((u64)&mbox) & ~0xF) | (channel & 0xF));

    while (*MBOX_STATUS & MBOX_FULL); // wait if mbox is full
    *MBOX_WRITE = r; // write address of our message with channel TO the mailbox
    
    while (1) { // now wait for the response
        while (*MBOX_STATUS & MBOX_EMPTY);

        if (r == *MBOX_READ) {
            _is_in_use = 0;
            return mbox[1] == MBOX_RESPONSE;
        }
    }
    _is_in_use = 0;
    return 0;
}

int mbox_property_exec() {
    return mbox_call(MBOX_CH_PROP);
}

void mbox_property_discard() {
    _is_in_use = 0;
}

u8 mbox_is_processing() {
    return _is_in_use;
}

// HELPER FUNCTIONS

void mbox_property_init() {
    mbox[0] = 1 * 4;
    mbox[1] = MBOX_REQUEST;
    _index = 2;
    _is_in_use = 1;
}

u32 mbox_property_tag(mbox_tag tag, u32 value_size_words, ...) {
    va_list vl;
    va_start(vl, value_size_words);

    mbox[_index++] = tag;
    mbox[_index++] = value_size_words * 4;
    mbox[_index++] = value_size_words * 4;
    u32 value_index = _index;

    u32 loop = value_size_words;
    for (u16 i = 0; i < loop; i++) {
        mbox[_index++] = va_arg(vl, u32);
    }

    va_end(vl);
    _is_in_use = 1;
    return value_index;
}

void mbox_property_end() {
    mbox[_index++] = MBOX_TAG_LAST;
    mbox[0] = _index * 4;
    _is_in_use = 1;
}

u32 get_mbox(u8 index) {
    return mbox[index];
}

void set_mbox(u8 index, u32 value) {
    mbox[index] = value;
}

u32 mbox_start() {
    mbox[0] = 1*4;
    mbox[1] = MBOX_REQUEST;
    return 2;
}

u32 mbox_tags(u32 index, u32 tag_id, u32 value_size_bytes, u32 values[]) {
    mbox[index++] = tag_id;
    mbox[index++] = value_size_bytes;
    mbox[index++] = value_size_bytes;
    u32 loop = value_size_bytes / (u32)4;
    for (u8 i = 0; i < loop; i++) { // values
        mbox[index++] = values[i];
    }
    return index;
}

u32 mbox_end(u32 index) {
    mbox[index++] = MBOX_TAG_LAST;
    return index;
}

void mbox_size(u32 count) {
    mbox[0] = count * 4;
}


u8 mbox_set_qpu(u32 state) {
    mbox_property_init();
    u32 i = mbox_property_tag(MBOX_TAG_GPU_SET_ENABLEQPU, 1, state);
    mbox_property_end();
    mbox_property_exec();

    return (u8)((mbox_property_exec() != 0) && (mbox[i] == 0));
}

u32 mbox_set_clock_rate(u32 clock_id, u32 rate_hz, u32 skip_turbo) {
    mbox_property_init();
    u32 i = mbox_property_tag(MBOX_TAG_SET_CLKRATE, 3, clock_id, rate_hz, skip_turbo);
    mbox_property_end();
    mbox_property_exec();

    return mbox[i+1];
}




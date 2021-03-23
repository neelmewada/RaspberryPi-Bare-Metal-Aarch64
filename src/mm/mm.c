#include "mm/mm.h"
#include "printf.h"

static u16 mem_map[PAGING_PAGES] = {0, };

u64 get_free_page() {
    for (u32 i = 0; i < PAGING_PAGES; i++) {
        if (mem_map[i] == 0) {
            mem_map[i] = 1;
            return LOW_MEMORY + i * PAGE_SIZE;
        }
    }
    return 0;
}

void free_page(u64 p) {
    mem_map[(p - LOW_MEMORY) / PAGE_SIZE] = 0;
}

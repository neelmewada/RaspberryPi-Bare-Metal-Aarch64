#pragma once

#include "peripherals/base.h"

#define PAGE_SHIFT	 		12
#define TABLE_SHIFT 		9
#define SECTION_SHIFT		(PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE   		(1 << PAGE_SHIFT)	
#define SECTION_SIZE		(1 << SECTION_SHIFT)	

#define LOW_MEMORY          (2 * SECTION_SIZE)
#define HIGH_MEMORY         MMIO_BASE

#define PAGING_MEMORY       (HIGH_MEMORY - LOW_MEMORY)
#define PAGING_PAGES        (PAGING_MEMORY / PAGE_SIZE)

#ifndef __ASSEMBLER__

#include "common.h"

u64 get_free_page();
void free_page(u64 p);
void memzero(unsigned long src, unsigned long n); // (u64, u64)

#endif

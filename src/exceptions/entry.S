#include "entry.h"

.macro VENTRY label
.align 7
    b    \label
.endm

.macro HANDLE_INVALID_ENTRY type
    KERNEL_ENTRY
    mov  x0, #\type
    mrs  x1, esr_el1
    mrs  x2, elr_el1
    bl   show_invalid_entry_message
    b    err_hang
.endm

.macro KERNEL_ENTRY
    sub	sp, sp, #S_FRAME_SIZE
	stp	x0, x1, [sp, #16 * 0]
	stp	x2, x3, [sp, #16 * 1]
	stp	x4, x5, [sp, #16 * 2]
	stp	x6, x7, [sp, #16 * 3]
	stp	x8, x9, [sp, #16 * 4]
	stp	x10, x11, [sp, #16 * 5]
	stp	x12, x13, [sp, #16 * 6]
	stp	x14, x15, [sp, #16 * 7]
	stp	x16, x17, [sp, #16 * 8]
	stp	x18, x19, [sp, #16 * 9]
	stp	x20, x21, [sp, #16 * 10]
	stp	x22, x23, [sp, #16 * 11]
	stp	x24, x25, [sp, #16 * 12]
	stp	x26, x27, [sp, #16 * 13]
	stp	x28, x29, [sp, #16 * 14]

    mrs	x22, elr_el1
	mrs	x23, spsr_el1

	stp	x30, x22, [sp, #16 * 15]
    str x23, [sp, #16 * 16]
.endm

.macro KERNEL_EXIT
    ldr	x23, [sp, #16 * 16]
	ldp	x30, x22, [sp, #16 * 15] 

	msr	elr_el1, x22			
	msr	spsr_el1, x23

    ldp	x0, x1, [sp, #16 * 0]
	ldp	x2, x3, [sp, #16 * 1]
	ldp	x4, x5, [sp, #16 * 2]
	ldp	x6, x7, [sp, #16 * 3]
	ldp	x8, x9, [sp, #16 * 4]
	ldp	x10, x11, [sp, #16 * 5]
	ldp	x12, x13, [sp, #16 * 6]
	ldp	x14, x15, [sp, #16 * 7]
	ldp	x16, x17, [sp, #16 * 8]
	ldp	x18, x19, [sp, #16 * 9]
	ldp	x20, x21, [sp, #16 * 10]
	ldp	x22, x23, [sp, #16 * 11]
	ldp	x24, x25, [sp, #16 * 12]
	ldp	x26, x27, [sp, #16 * 13]
	ldp	x28, x29, [sp, #16 * 14]
	add	sp, sp, #S_FRAME_SIZE
	eret
.endm

// Exception Vector Table
.align	11
.globl vectors 
vectors:
	VENTRY	sync_invalid_el1t			// Synchronous EL1t
	VENTRY	irq_invalid_el1t			// IRQ EL1t
	VENTRY	fiq_invalid_el1t			// FIQ EL1t
	VENTRY	error_invalid_el1t			// SError EL1t

	VENTRY	sync_invalid_el1h			// Synchronous EL1h
	VENTRY	el1h_irq					// IRQ EL1h
	VENTRY	fiq_invalid_el1h			// FIQ EL1h
	VENTRY	error_invalid_el1h			// SError EL1h

	VENTRY	sync_invalid_el0_64			// Synchronous 64-bit EL0
	VENTRY	irq_invalid_el0_64			// IRQ 64-bit EL0
	VENTRY	fiq_invalid_el0_64			// FIQ 64-bit EL0
	VENTRY	error_invalid_el0_64		// SError 64-bit EL0

	VENTRY	sync_invalid_el0_32			// Synchronous 32-bit EL0
	VENTRY	irq_invalid_el0_32			// IRQ 32-bit EL0
	VENTRY	fiq_invalid_el0_32			// FIQ 32-bit EL0
	VENTRY	error_invalid_el0_32		// SError 32-bit EL0

sync_invalid_el1t:
	HANDLE_INVALID_ENTRY  SYNC_INVALID_EL1t

irq_invalid_el1t:
	HANDLE_INVALID_ENTRY  IRQ_INVALID_EL1t

fiq_invalid_el1t:
	HANDLE_INVALID_ENTRY  FIQ_INVALID_EL1t

error_invalid_el1t:
	HANDLE_INVALID_ENTRY  ERROR_INVALID_EL1t

sync_invalid_el1h:
	HANDLE_INVALID_ENTRY  SYNC_INVALID_EL1h

fiq_invalid_el1h:
	HANDLE_INVALID_ENTRY  FIQ_INVALID_EL1h

error_invalid_el1h:
	HANDLE_INVALID_ENTRY  ERROR_INVALID_EL1h

sync_invalid_el0_64:
	HANDLE_INVALID_ENTRY  SYNC_INVALID_EL0_64

irq_invalid_el0_64:
	HANDLE_INVALID_ENTRY  IRQ_INVALID_EL0_64

fiq_invalid_el0_64:
	HANDLE_INVALID_ENTRY  FIQ_INVALID_EL0_64

error_invalid_el0_64:
	HANDLE_INVALID_ENTRY  ERROR_INVALID_EL0_64

sync_invalid_el0_32:
	HANDLE_INVALID_ENTRY  SYNC_INVALID_EL0_32

irq_invalid_el0_32:
	HANDLE_INVALID_ENTRY  IRQ_INVALID_EL0_32

fiq_invalid_el0_32:
	HANDLE_INVALID_ENTRY  FIQ_INVALID_EL0_32

error_invalid_el0_32:
	HANDLE_INVALID_ENTRY  ERROR_INVALID_EL0_32


el1h_irq:
    KERNEL_ENTRY
    bl   handle_irq
    KERNEL_EXIT

.globl ret_from_fork
ret_from_fork:
    bl   schedule_tail
    mov  x0, x20
    blr  x19    // should never return

.global err_hang
err_hang:
    b   err_hang

#include "mm/mm.h"
#include "sysregs.h"

.section ".text.boot"

.global _start
_start:
    mrs    x0, mpidr_el1
    and    x0, x0, 0xFF   // check processor core id
    cbz    x0, master     // execute master only if it's 1st core
    b      proc_hang

master:
    // enable access to floating point & SIMD
    ldr    x0, =CPACR_EL1_VALUE
    msr    cpacr_el1, x0

    ldr    x0, =CPTR_EL2_VALUE
    msr    cptr_el2, x0

    ldr    x0, =CPTR_EL3_VALUE
    msr    cptr_el3, x0

    // switch from el3 to el1
    ldr    x0, =SCTLR_VALUE_MMU_DISABLED
    msr    sctlr_el1, x0

    ldr    x0, =HCR_VALUE
    msr    hcr_el2, x0

    ldr    x0, =SCR_VALUE
    msr    scr_el3, x0

    ldr    x0, =SPSR_VALUE
    msr    spsr_el3, x0

    adr    x0, el1_entry
    msr    elr_el3, x0
    
    eret

el1_entry:
    adr    x0, __bss_start
    adr    x1, __bss_end
    sub    x1, x1, x0
    bl     memzero

    mov    sp, #LOW_MEMORY

    bl     kernel_main
    b      proc_hang

proc_hang:
    wfe
    b      proc_hang

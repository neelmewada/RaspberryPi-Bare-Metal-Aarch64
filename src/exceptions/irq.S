.globl irq_init_vectors
irq_init_vectors:
	adr	  x0, vectors		// load VBAR_EL1 with virtual
	msr	  vbar_el1, x0	    // vector table address
	ret

.globl irq_enable
irq_enable:
	msr   daifclr, #2 
	ret

.globl irq_disable
irq_disable:
	msr	  daifset, #2
	ret


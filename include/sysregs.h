#pragma once

#define SCTLR_RESERVED                  (3 << 28) | (3 << 22) | (1 << 20) | (1 << 11)
#define SCTLR_EE_LITTLE_ENDIAN          (0 << 25)
#define SCTLR_EOE_LITTLE_ENDIAN         (0 << 24)
#define SCTLR_I_CACHE_DISABLED          (0 << 12)
#define SCTLR_D_CACHE_DISABLED          (0 << 2)
#define SCTLR_MMU_DISABLED              (0 << 0)
#define SCTLR_MMU_ENABLED               (1 << 0)
#define SCTLR_VALUE_MMU_DISABLED	(SCTLR_RESERVED | SCTLR_EE_LITTLE_ENDIAN | SCTLR_I_CACHE_DISABLED | SCTLR_D_CACHE_DISABLED | SCTLR_MMU_DISABLED)

#define HCR_RW	    			    (1 << 31) // set EL1 to aarch64
#define HCR_VALUE			        HCR_RW    // hcr_el2

#define SCR_RESERVED	    		(3 << 4)
#define SCR_RW				        (1 << 10) // set EL2 to aarch64
#define SCR_NS				        (1 << 0)  // set EL0-2 to non-secure state
#define SCR_VALUE	    	    	(SCR_RESERVED | SCR_RW | SCR_NS) // scr_el3

#define SPSR_MASK_ALL 			    (7 << 6)
#define SPSR_EL1h			        (5 << 0)
#define SPSR_EL2h			        (9 << 0)
#define SPSR_VALUE			        (SPSR_MASK_ALL | SPSR_EL1h)

// Enable Floating point access

#define CPACR_FPEN                  (3 << 20) // floating point access
#define CPACR_ZEN                   (3 << 16)
#define CPACR_EL1_VALUE             (CPACR_FPEN | CPACR_ZEN)

#define CPTR_EL2_TFP                (0 << 10)
#define CPTR_EL2_TZ                 (0 << 8)
#define CPTR_EL2_VALUE              (CPTR_EL2_TFP | CPTR_EL2_TZ)

#define CPTR_EL3_TFP                (0 << 10)
#define CPTR_EL3_VALUE              (CPTR_EL3_TFP)



.globl memzero
memzero:
    str    xzr, [x0], #8 // store 0 at addr[x0]
    subs   x1, x1, #8    // x1 = x1 - 8 ;  8 = 64bits
    b.gt   memzero       // if x1 > 0, keep looping
    ret

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "lfsr.h"

void lfsr_calculate(uint16_t *reg) {
    unsigned x0 = *reg & 1;
    unsigned x2 = (*reg >> 2) & 1;
    unsigned x3 = (*reg >> 3) & 1;
    unsigned x5 = (*reg >> 5) & 1;

    *reg >>= 1;
    unsigned xn = x0 ^ x2 ^ x3 ^ x5;
    
    *reg |= (xn << 15);
}


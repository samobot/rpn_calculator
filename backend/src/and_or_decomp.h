#ifndef AND_OR_DECOMP_H
#define AND_OR_DECOMP_H

#include <stdint.h>

typedef struct {
    uint64_t and_p;
    uint64_t or_p;
} and_or_decomp_t;

void and_or_decomp(and_or_decomp_t *result, uint64_t value, uint64_t bitmask);

#endif
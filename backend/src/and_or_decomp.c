#include "and_or_decomp.h"

void and_or_decomp(and_or_decomp_t *result, uint64_t value, uint64_t bitmask) {
    result->or_p = value & bitmask;
    result->and_p = value | ~bitmask;
}
#ifndef PROGRAMMER_RPN_H
#define PROGRAMMER_RPN_H

#include <stdint.h>

typedef struct {
    uint64_t X_reg;
    uint64_t Y_reg;
    uint64_t Z_reg;
    uint64_t T_reg;
} programmer_rpn_t;

void programmer_rpn_init(programmer_rpn_t *s);
void programmer_rpn_push(programmer_rpn_t *s, uint64_t value);
uint64_t programmer_rpn_pop(programmer_rpn_t *s);

#endif
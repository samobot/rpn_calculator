#include "programmer_rpn.h"

void programmer_rpn_init(programmer_rpn_t *s) {
    s->X_reg = 0;
    s->Y_reg = 0;
    s->Z_reg = 0;
    s->T_reg = 0;
}

void programmer_rpn_push(programmer_rpn_t *s, uint64_t value) {
    s->T_reg = s->Z_reg;
    s->Z_reg = s->Y_reg;
    s->Y_reg = s->X_reg;
    s->X_reg = value;
}

uint64_t programmer_rpn_pop(programmer_rpn_t *s) {
    uint64_t tmp = s->X_reg;
    s->X_reg = s->Y_reg;
    s->Y_reg = s->Z_reg;
    s->Z_reg = s->T_reg;
    return tmp;
}
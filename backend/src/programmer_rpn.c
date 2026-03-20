#include "programmer_rpn.h"

#define ALL_ONES ((uint64_t)~(0L)) // this generates all 1's

void apply_ws_bitmask(programmer_rpn_t *s) {
    uint64_t bitmask = ALL_ONES>>(64-s->word_size);
    s->X_reg &= bitmask;
    s->Y_reg &= bitmask;
    s->Z_reg &= bitmask;
    s->T_reg &= bitmask;
}

void programmer_rpn_init(programmer_rpn_t *s) {
    s->X_reg = 0;
    s->Y_reg = 0;
    s->Z_reg = 0;
    s->T_reg = 0;
    s->word_size = PROGRAMMER_RPN_WS_64;
    s->sign_mode = PROGRAMMER_RPN_2S_COMP;
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

void programmer_rpn_set_ws(programmer_rpn_t *s, programmer_rpn_ws_t ws) {
    switch(ws) {
        case PROGRAMMER_RPN_WS_8:
        case PROGRAMMER_RPN_WS_16:
        case PROGRAMMER_RPN_WS_32:
        case PROGRAMMER_RPN_WS_64:
            s->word_size = ws;
            apply_ws_bitmask(s);
    }
}

void programmer_rpn_or(programmer_rpn_t *s) {
    uint64_t x = programmer_rpn_pop(s);
    uint64_t y = programmer_rpn_pop(s);
    programmer_rpn_push(s, x|y);
    apply_ws_bitmask(s);
}

void programmer_rpn_ori(programmer_rpn_t *s, uint64_t e_reg) {
    s->X_reg |= e_reg;
    apply_ws_bitmask(s);
}

void programmer_rpn_not(programmer_rpn_t *s) {
    s->X_reg = ~(s->X_reg);
    apply_ws_bitmask(s);
}

void programmer_rpn_and(programmer_rpn_t *s) {
    uint64_t x = programmer_rpn_pop(s);
    uint64_t y = programmer_rpn_pop(s);
    programmer_rpn_push(s, x&y);
    apply_ws_bitmask(s);
}

void programmer_rpn_andi(programmer_rpn_t *s, uint64_t e_reg) {
    s->X_reg &= e_reg;
    apply_ws_bitmask(s);
}

void programmer_rpn_xor(programmer_rpn_t *s) {
    uint64_t x = programmer_rpn_pop(s);
    uint64_t y = programmer_rpn_pop(s);
    programmer_rpn_push(s, x^y);
    apply_ws_bitmask(s);
}

void programmer_rpn_xori(programmer_rpn_t *s, uint64_t e_reg) {
    s->X_reg ^= e_reg;
    apply_ws_bitmask(s);
}

void programmer_rpn_setn(programmer_rpn_t *s) {
    apply_ws_bitmask(s); // apply bitmask first so n is correct
    uint64_t x = programmer_rpn_pop(s);
    if(x < s->word_size) { // if x less than word size, set bit in y, otherwise ignore
        uint64_t y = programmer_rpn_pop(s);
        programmer_rpn_push(s, y | (1<<x));
    } 
}

void programmer_rpn_setni(programmer_rpn_t *s, uint64_t e_reg) {
    s->X_reg |= (1<<e_reg);
    apply_ws_bitmask(s);
}

void programmer_rpn_clrn(programmer_rpn_t *s) {
    apply_ws_bitmask(s); // apply bitmask first so n is correct
    uint64_t x = programmer_rpn_pop(s);
    if(x < s->word_size) { // if x less than word size, set bit in y, otherwise ignore
        uint64_t y = programmer_rpn_pop(s);
        programmer_rpn_push(s, y & (ALL_ONES^(1<<x)));
    } 
}

void programmer_rpn_clrni(programmer_rpn_t *s, uint64_t e_reg) {
    s->X_reg &= ALL_ONES^(1<<e_reg);
    apply_ws_bitmask(s);
}

void programmer_rpn_rotate_down(programmer_rpn_t *s) {
    s->T_reg = programmer_rpn_pop(s);
}

void programmer_rpn_rotate_up(programmer_rpn_t *s) {
    programmer_rpn_push(s, s->T_reg);
}

/*void programmer_rpn_add(programmer_rpn_t *s) {
    uint64_t x = programmer_rpn_pop(s);
    uint64_t y = programmer_rpn_pop(s);
    programmer_rpn_push(s, x+y);
    apply_ws_bitmask(s);
}*/
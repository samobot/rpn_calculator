#include "programmer_rpn.h"

#include <cstdint>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

#define ALL_ONES ((uint64_t)~(0L)) // this generates all 1's

void apply_ws_bitmask(programmer_rpn_t *s) {
    uint64_t bitmask = ALL_ONES>>(64-s->word_size);
    s->X_reg &= bitmask;
    s->Y_reg &= bitmask;
    s->Z_reg &= bitmask;
    s->T_reg &= bitmask;
}

void sign_extend(uint64_t *x, programmer_rpn_ws_t ws) {
    if(*x & 1<<(ws-1)) {
        *x |= ALL_ONES << (ws-1);
    }
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
            if(s->sign_mode == PROGRAMMER_RPN_2S_COMP) {
                sign_extend(&s->X_reg, s->word_size);
                sign_extend(&s->Y_reg, s->word_size);
                sign_extend(&s->Z_reg, s->word_size);
                sign_extend(&s->T_reg, s->word_size);
            }
            s->word_size = ws;
            apply_ws_bitmask(s);
    }
}

void programmer_rpn_set_signmode(programmer_rpn_t *s, programmer_rpn_signmode_t signmode) {
    switch(signmode) {
        case PROGRAMMER_RPN_2S_COMP:
        case PROGRAMMER_RPN_UNSIGNED:
            s->sign_mode = signmode;
            apply_ws_bitmask(s);
    }
}

void programmer_rpn_enter(programmer_rpn_t *s, uint64_t e_reg) {
    programmer_rpn_push(s, e_reg);
    apply_ws_bitmask(s);
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

void programmer_rpn_swap_XY(programmer_rpn_t *s) {
    uint64_t tmp = s->X_reg;
    s->X_reg = s->Y_reg;
    s->Y_reg = tmp;
}

void programmer_rpn_clear_X(programmer_rpn_t *s) {
    s->X_reg = 0;
}

void programmer_rpn_clear_all(programmer_rpn_t *s) {
    s->X_reg = 0;
    s->Y_reg = 0;
    s->Z_reg = 0;
    s->T_reg = 0;
}

void programmer_rpn_override_X(programmer_rpn_t *s, uint64_t e_reg) {
    s->X_reg = e_reg;
    apply_ws_bitmask(s);
}

void programmer_rpn_change_sign(programmer_rpn_t *s) {
    s->X_reg = 0-s->X_reg;
    apply_ws_bitmask(s);
}

void programmer_rpn_lshift(programmer_rpn_t *s) {
    s->X_reg <<= 1;
    apply_ws_bitmask(s);
}

void programmer_rpn_rshift(programmer_rpn_t *s) {
    s->X_reg >>= 1;
    apply_ws_bitmask(s);
}

void programmer_rpn_lshiftn(programmer_rpn_t *s) {
    apply_ws_bitmask(s);
    uint64_t x = programmer_rpn_pop(s);
    uint64_t y = programmer_rpn_pop(s);
    programmer_rpn_push(s, y<<x);
    apply_ws_bitmask(s);
}

void programmer_rpn_lshiftni(programmer_rpn_t *s, uint64_t e_reg) {
    s->X_reg <<= e_reg;
    apply_ws_bitmask(s);
}

void programmer_rpn_rshiftn(programmer_rpn_t *s) {
    apply_ws_bitmask(s);
    uint64_t x = programmer_rpn_pop(s);
    uint64_t y = programmer_rpn_pop(s);
    programmer_rpn_push(s, y>>x);
    apply_ws_bitmask(s);
}

void programmer_rpn_rshiftni(programmer_rpn_t *s, uint64_t e_reg) {
    s->X_reg >>= e_reg;
    apply_ws_bitmask(s);
}

void programmer_rpn_lroll(programmer_rpn_t *s) {
    uint8_t topbit = !!(s->X_reg & (1<<(s->word_size-1)));
    s->X_reg = (s->X_reg << 1) | topbit;
    apply_ws_bitmask(s);
}

void programmer_rpn_rroll(programmer_rpn_t *s) {
    uint8_t bottombit = s->X_reg & 1;
    s->X_reg = (s->X_reg >> 1) | (bottombit << (s->word_size-1));
    apply_ws_bitmask(s);
}

void programmer_rpn_lrolln(programmer_rpn_t *s) {
    apply_ws_bitmask(s);
    uint64_t x = programmer_rpn_pop(s) % s->word_size;
    uint64_t y = programmer_rpn_pop(s);
    uint64_t topbits = y & (ALL_ONES << (s->word_size-x));
    y <<= x;
    y |= (topbits >> (s->word_size-x));
    programmer_rpn_push(s, y);
    apply_ws_bitmask(s);
}

void programmer_rpn_lrollni(programmer_rpn_t *s, uint64_t e_reg) {
    e_reg %= s->word_size;
    uint64_t topbits = s->X_reg & (ALL_ONES << (s->word_size - e_reg));
    s->X_reg <<= e_reg;
    s->X_reg |= (topbits >> (s->word_size-e_reg));
    apply_ws_bitmask(s);
}

void programmer_rpn_rrolln(programmer_rpn_t *s) {
    apply_ws_bitmask(s);
    uint64_t x = programmer_rpn_pop(s) % s->word_size;
    uint64_t y = programmer_rpn_pop(s);
    uint64_t bottombits = y & (ALL_ONES >> (s->word_size-x));
    y >>= x;
    y |= (bottombits << (s->word_size-x));
    programmer_rpn_push(s, y);
    apply_ws_bitmask(s);
}

void programmer_rpn_rrollni(programmer_rpn_t *s, uint64_t e_reg) {
    e_reg %= s->word_size;
    uint64_t bottombits = s->X_reg & (ALL_ONES >> (s->word_size-e_reg));
    s->X_reg >>= e_reg;
    s->X_reg |= (bottombits << (s->word_size-e_reg));
    apply_ws_bitmask(s);
}

void programmer_rpn_add(programmer_rpn_t *s) {
    apply_ws_bitmask(s);
    uint64_t x = programmer_rpn_pop(s);
    uint64_t y = programmer_rpn_pop(s);
    programmer_rpn_push(s, x+y);
    apply_ws_bitmask(s);
}

void programmer_rpn_addi(programmer_rpn_t *s, uint64_t e_reg) {
    s->X_reg += e_reg;
    apply_ws_bitmask(s);
}

void programmer_rpn_sub(programmer_rpn_t *s) {
    apply_ws_bitmask(s);
    uint64_t x = programmer_rpn_pop(s);
    uint64_t y = programmer_rpn_pop(s);
    programmer_rpn_push(s, y-x);
    apply_ws_bitmask(s);
}

void programmer_rpn_subi(programmer_rpn_t *s, uint64_t e_reg) {
    s->X_reg -= e_reg;
    apply_ws_bitmask(s);
}

void programmer_rpn_mul(programmer_rpn_t *s) {
    uint64_t x = programmer_rpn_pop(s);
    uint64_t y = programmer_rpn_pop(s);
    programmer_rpn_push(s, x*y);
    apply_ws_bitmask(s);
}

void programmer_rpn_muli(programmer_rpn_t *s, uint64_t e_reg) {
    s->X_reg *= e_reg;
    apply_ws_bitmask(s);
}

void programmer_rpn_div(programmer_rpn_t *s) {
    uint64_t x = programmer_rpn_pop(s);
    uint64_t y = programmer_rpn_pop(s);
    uint64_t res = 0;
    if(s->sign_mode == PROGRAMMER_RPN_UNSIGNED) {
        res = y/x;
    } else if(s->sign_mode == PROGRAMMER_RPN_2S_COMP) {
        sign_extend(&x, s->word_size);
        sign_extend(&y, s->word_size);
        res = (uint64_t)(((int64_t)y)/((int64_t)x));
    }
    programmer_rpn_push(s, res);
    apply_ws_bitmask(s);
}

void programmer_rpn_divi(programmer_rpn_t *s, uint64_t e_reg) {
    if(s->sign_mode == PROGRAMMER_RPN_UNSIGNED) {
        s->X_reg /= e_reg;
    } else if(s->sign_mode == PROGRAMMER_RPN_2S_COMP) {
        sign_extend(&s->X_reg, s->word_size);
        sign_extend(&e_reg, s->word_size);
        s->X_reg = (uint64_t)(((int64_t)s->X_reg)/((int64_t)e_reg));
    }
    apply_ws_bitmask(s);
}

void programmer_rpn_dwmul(programmer_rpn_t *s) {
    if(s->word_size == PROGRAMMER_RPN_WS_64) {
        uint8_t negative = 0;
        if(s->sign_mode == PROGRAMMER_RPN_2S_COMP) {
            negative = !!((s->X_reg & (ALL_ONES << 63)) ^ (s->Y_reg & (ALL_ONES << 63)));
            if(s->X_reg & (ALL_ONES << 63)) s->X_reg = ((~s->X_reg) + 1);
            if(s->Y_reg & (ALL_ONES << 63)) s->Y_reg = ((~s->Y_reg) + 1);
        }
        uint64_t xlow = s->X_reg & (ALL_ONES >> 32);
        uint64_t xhigh = (s->X_reg & (ALL_ONES << 32)) >> 32;
        uint64_t ylow = s->Y_reg & (ALL_ONES >> 32);
        uint64_t yhigh = (s->Y_reg & (ALL_ONES << 32)) >> 32;

        uint64_t xy = xlow*yhigh;
        uint64_t yx = ylow*xhigh;
        uint64_t reshigh = xhigh * yhigh + (xy >> 32) + (yx >> 32);
        uint64_t reslow = xlow * ylow;
        if(__builtin_add_overflow(reslow, xy << 32, &reslow)) reshigh++;
        if(__builtin_add_overflow(reslow, yx << 32, &reslow)) reshigh++;
        if(negative) {
            reslow = ~reslow;
            reshigh = ~reshigh;
            if(__builtin_add_overflow(reslow, 1, &reslow)) reshigh++;
        }
        s->X_reg = reslow;
        s->Y_reg = reshigh;
        return;
    }
    uint64_t res = 0;
    if(s->sign_mode == PROGRAMMER_RPN_2S_COMP) {
        sign_extend(&s->X_reg, s->word_size);
        sign_extend(&s->Y_reg, s->word_size);
    }
    res = s->X_reg*s->Y_reg;
    s->Y_reg = res >> s->word_size;
    s->X_reg = res;
    apply_ws_bitmask(s);
}

void programmer_rpn_dwmuli(programmer_rpn_t *s, uint64_t e_reg) {
    if(s->word_size == PROGRAMMER_RPN_WS_64) {
        uint8_t negative = 0;
        if(s->sign_mode == PROGRAMMER_RPN_2S_COMP) {
            negative = !!((s->X_reg & (ALL_ONES << 63)) ^ (s->Y_reg & (ALL_ONES << 63)));
            if(s->X_reg & (ALL_ONES << 63)) s->X_reg = ((~s->X_reg) + 1);
            if(e_reg & (ALL_ONES << 63)) e_reg = ((~e_reg) + 1);
        }
        uint64_t xlow = s->X_reg & (ALL_ONES >> 32);
        uint64_t xhigh = (s->X_reg & (ALL_ONES << 32)) >> 32;
        uint64_t ylow = e_reg & (ALL_ONES >> 32);
        uint64_t yhigh = (e_reg & (ALL_ONES << 32)) >> 32;

        uint64_t xy = xlow*yhigh;
        uint64_t yx = ylow*xhigh;
        uint64_t reshigh = xhigh * yhigh + (xy >> 32) + (yx >> 32);
        uint64_t reslow = xlow * ylow;
        if(__builtin_add_overflow(reslow, xy << 32, &reslow)) reshigh++;
        if(__builtin_add_overflow(reslow, yx << 32, &reslow)) reshigh++;
        if(negative) {
            reslow = ~reslow;
            reshigh = ~reshigh;
            if(__builtin_add_overflow(reslow, 1, &reslow)) reshigh++;
        }
        s->X_reg = reshigh;
        programmer_rpn_push(s, reslow);
        return;
    }
    uint64_t x = programmer_rpn_pop(s);
    uint64_t res = 0;
    if(s->sign_mode == PROGRAMMER_RPN_2S_COMP) {
        sign_extend(&x, s->word_size);
        sign_extend(&e_reg, s->word_size);
    }
    res = x*e_reg;
    programmer_rpn_push(s, res >> s->word_size);
    programmer_rpn_push(s, res);
    apply_ws_bitmask(s);
}

void programmer_rpn_mod(programmer_rpn_t *s) {
    uint64_t x = programmer_rpn_pop(s);
    uint64_t y = programmer_rpn_pop(s);
    if(s->sign_mode == PROGRAMMER_RPN_2S_COMP) {
        sign_extend(&x, s->word_size);
        sign_extend(&y, s->word_size);
        uint64_t result = (uint64_t)(((((int64_t)y) % ((int64_t)x)) + (int64_t)(x)) % (int64_t)x);
        programmer_rpn_push(s, result);
    } else {
        programmer_rpn_push(s, y % x);
    }
    apply_ws_bitmask(s);
}

void programmer_rpn_modi(programmer_rpn_t *s, uint64_t e_reg) {
    if(s->sign_mode == PROGRAMMER_RPN_2S_COMP) {
        sign_extend(&s->X_reg, s->word_size);
        sign_extend(&e_reg, s->word_size);
        uint64_t result = (uint64_t)(((((int64_t)s->X_reg) % ((int64_t)e_reg)) + (int64_t)(e_reg)) % (int64_t)e_reg);
        programmer_rpn_push(s, result);
    } else {
        s->X_reg %= e_reg;
    }
    apply_ws_bitmask(s);
}
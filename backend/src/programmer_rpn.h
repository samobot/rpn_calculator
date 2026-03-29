#ifndef PROGRAMMER_RPN_H
#define PROGRAMMER_RPN_H

#include <cstdint>
#include <stdint.h>

typedef enum {
    PROGRAMMER_RPN_WS_8 = 8,
    PROGRAMMER_RPN_WS_16 = 16,
    PROGRAMMER_RPN_WS_32 = 32,
    PROGRAMMER_RPN_WS_64 = 64,
} programmer_rpn_ws_t;

typedef enum {
    PROGRAMMER_RPN_UNSIGNED = 0,
    PROGRAMMER_RPN_2S_COMP = 1,
} programmer_rpn_signmode_t;

typedef struct {
    uint64_t X_reg;
    uint64_t Y_reg;
    uint64_t Z_reg;
    uint64_t T_reg;
    programmer_rpn_ws_t word_size;
    programmer_rpn_signmode_t sign_mode;
} programmer_rpn_t;

void programmer_rpn_init(programmer_rpn_t *s);
void programmer_rpn_push(programmer_rpn_t *s, uint64_t value);
uint64_t programmer_rpn_pop(programmer_rpn_t *s);
void programmer_rpn_set_ws(programmer_rpn_t *s, programmer_rpn_ws_t ws);
void programmer_rpn_set_signmode(programmer_rpn_t *s, programmer_rpn_signmode_t signmode);
void programmer_rpn_enter(programmer_rpn_t *s, uint64_t e_reg);
void programmer_rpn_or(programmer_rpn_t *s);
void programmer_rpn_ori(programmer_rpn_t *s, uint64_t e_reg);
void programmer_rpn_not(programmer_rpn_t *s);
void programmer_rpn_and(programmer_rpn_t *s);
void programmer_rpn_andi(programmer_rpn_t *s, uint64_t e_reg);
void programmer_rpn_xor(programmer_rpn_t *s);
void programmer_rpn_xori(programmer_rpn_t *s, uint64_t e_reg);
void programmer_rpn_add(programmer_rpn_t *s);
void programmer_rpn_setn(programmer_rpn_t *s);
void programmer_rpn_setni(programmer_rpn_t *s, uint64_t e_reg);
void programmer_rpn_clrn(programmer_rpn_t *s);
void programmer_rpn_clrni(programmer_rpn_t *s, uint64_t e_reg);
void programmer_rpn_rotate_down(programmer_rpn_t *s);
void programmer_rpn_rotate_up(programmer_rpn_t *s);
void programmer_rpn_swap_XY(programmer_rpn_t *s);
void programmer_rpn_clear_X(programmer_rpn_t *s);
void programmer_rpn_clear_all(programmer_rpn_t *s);
void programmer_rpn_override_X(programmer_rpn_t *s, uint64_t e_reg);
void programmer_rpn_change_sign(programmer_rpn_t *s);
void programmer_rpn_lshift(programmer_rpn_t *s);
void programmer_rpn_rshift(programmer_rpn_t *s);
void programmer_rpn_lshiftn(programmer_rpn_t *s);
void programmer_rpn_lshiftni(programmer_rpn_t *s, uint64_t e_reg);
void programmer_rpn_rshiftn(programmer_rpn_t *s);
void programmer_rpn_rshiftni(programmer_rpn_t *s, uint64_t e_reg);
void programmer_rpn_lroll(programmer_rpn_t *s);
void programmer_rpn_rroll(programmer_rpn_t *s);
void programmer_rpn_lrolln(programmer_rpn_t *s);
void programmer_rpn_lrollni(programmer_rpn_t *s, uint64_t e_reg);
void programmer_rpn_rrolln(programmer_rpn_t *s);
void programmer_rpn_rrollni(programmer_rpn_t *s, uint64_t e_reg);
void programmer_rpn_add(programmer_rpn_t *s);
void programmer_rpn_addi(programmer_rpn_t *s, uint64_t e_reg);
void programmer_rpn_sub(programmer_rpn_t *s);
void programmer_rpn_subi(programmer_rpn_t *s, uint64_t e_reg);
void programmer_rpn_mul(programmer_rpn_t *s);
void programmer_rpn_muli(programmer_rpn_t *s, uint64_t e_reg);
void programmer_rpn_div(programmer_rpn_t *s);
void programmer_rpn_divi(programmer_rpn_t *s, uint64_t e_reg);
void programmer_rpn_dwmul(programmer_rpn_t *s);
void programmer_rpn_dwmuli(programmer_rpn_t *s, uint64_t e_reg);
void programmer_rpn_mod(programmer_rpn_t *s);
void programmer_rpn_modi(programmer_rpn_t *s, uint64_t e_reg);

#endif
#include "backbone.h"
#include "backbone_utils.h"
#include "programmer_rpn.h"
#include <stdbool.h>

void backbone_add_to_e(backbone_t *backbone, uint8_t value) {
    if(backbone->base != BACKBONE_BASE_ASCII && value < backbone->base) {
        backbone->e_reg = backbone->base*backbone->e_reg + value;
    }
}

void backbone_backspace_e(backbone_t *backbone) {
    if(backbone->base != BACKBONE_BASE_ASCII) {
        backbone->e_reg /= backbone->base;
    }
}

void backbone_init(backbone_t *backbone, i_frontend_t *frontend) {
    programmer_rpn_init(&backbone->rpn);
    backbone->frontend = frontend;
    backbone->frontend->frontend_init(&backbone->rpn);
    backbone->e_reg = 0;
    backbone->base = BACKBONE_BASE_HEX;
    backbone->shift = BACKBONE_SHIFT_NONE;
    backbone->frontend->frontend_refresh(backbone->e_reg, backbone->e_reg, backbone->base, backbone->shift);
}

void backbone_key(backbone_t *backbone, backbone_key_t key) {
    bool consume_shift = 1;

    switch(key) {
        case BACKBONE_KEY_UP:
            if(backbone->shift == BACKBONE_SHIFT_UP) backbone->shift = BACKBONE_SHIFT_NONE;
            else backbone->shift = BACKBONE_SHIFT_UP;
            consume_shift = 0;
            break;
        case BACKBONE_KEY_DOWN:
            if(backbone->shift == BACKBONE_SHIFT_DOWN) backbone->shift = BACKBONE_SHIFT_NONE;
            else backbone->shift = BACKBONE_SHIFT_DOWN;
            consume_shift = 0;
            break;
        case BACKBONE_KEY_OCT:
            backbone->base = BACKBONE_BASE_OCT;
            break;
        case BACKBONE_KEY_HEX:
            if(backbone->shift == BACKBONE_SHIFT_UP) backbone->base = BACKBONE_BASE_ASCII;
            else backbone->base = BACKBONE_BASE_HEX;
            break;
        case BACKBONE_KEY_DEC:
            backbone->base = BACKBONE_BASE_DEC;
            break;
        case BACKBONE_KEY_BIN:
            backbone->base = BACKBONE_BASE_BIN;
            break;
        case BACKBONE_KEY_2:
            if(backbone->shift == BACKBONE_SHIFT_UP) programmer_rpn_set_signmode(&backbone->rpn, PROGRAMMER_RPN_2S_COMP);
            else backbone_add_to_e(backbone, key);
            break;
        case BACKBONE_KEY_3:
            if(backbone->shift == BACKBONE_SHIFT_UP) programmer_rpn_set_signmode(&backbone->rpn, PROGRAMMER_RPN_UNSIGNED);
            else backbone_add_to_e(backbone, key);
            break;
        case BACKBONE_KEY_0:
        case BACKBONE_KEY_1:
        case BACKBONE_KEY_4:
        case BACKBONE_KEY_5:
        case BACKBONE_KEY_6:
        case BACKBONE_KEY_7:
        case BACKBONE_KEY_8:
        case BACKBONE_KEY_9:
        case BACKBONE_KEY_A:
        case BACKBONE_KEY_B:
        case BACKBONE_KEY_C:
        case BACKBONE_KEY_D:
        case BACKBONE_KEY_E:
        case BACKBONE_KEY_F:
            backbone_add_to_e(backbone, key);
            break;
        case BACKBONE_KEY_BACK:
            backbone_backspace_e(backbone);
            break;
        case BACKBONE_KEY_ESC:
            backbone->e_reg = 0;
            break;
        case BACKBONE_KEY_ENTER:
            programmer_rpn_enter(&backbone->rpn, backbone->e_reg);
            backbone->e_reg = 0;
            break;
        case BACKBONE_KEY_OVRX:
            programmer_rpn_override_X(&backbone->rpn, backbone->e_reg);
            backbone->e_reg = 0;
            break;
        case BACKBONE_KEY_CHS:
            programmer_rpn_change_sign(&backbone->rpn);
            break;
        case BACKBONE_KEY_ROTATE:
            if(backbone->shift == BACKBONE_SHIFT_UP) programmer_rpn_rotate_up(&backbone->rpn);
            else programmer_rpn_rotate_down(&backbone->rpn);
            break;
        case BACKBONE_KEY_LR:
            if(backbone->shift == BACKBONE_SHIFT_UP) {
                if(backbone->e_reg) programmer_rpn_lrollni(&backbone->rpn, backbone->e_reg);
                else programmer_rpn_lrolln(&backbone->rpn);
                backbone->e_reg = 0;
            } else programmer_rpn_lroll(&backbone->rpn);
            break;
        case BACKBONE_KEY_RR:
            if(backbone->shift == BACKBONE_SHIFT_UP) {
                if(backbone->e_reg) programmer_rpn_rrollni(&backbone->rpn, backbone->e_reg);
                else programmer_rpn_rrolln(&backbone->rpn);
                backbone->e_reg = 0;
            } else programmer_rpn_rroll(&backbone->rpn);
            break;
        case BACKBONE_KEY_LS:
            if(backbone->shift == BACKBONE_SHIFT_UP) {
                if(backbone->e_reg) programmer_rpn_lshiftni(&backbone->rpn, backbone->e_reg);
                else programmer_rpn_lshiftn(&backbone->rpn);
                backbone->e_reg = 0;
            } else programmer_rpn_lshift(&backbone->rpn);
            break;
        case BACKBONE_KEY_RS:
            if(backbone->shift == BACKBONE_SHIFT_UP) {
                if(backbone->e_reg) programmer_rpn_rshiftni(&backbone->rpn, backbone->e_reg);
                else programmer_rpn_rshiftn(&backbone->rpn);
                backbone->e_reg = 0;
            } else programmer_rpn_rshift(&backbone->rpn);
            break;
        case BACKBONE_KEY_SET:
            programmer_rpn_setni(&backbone->rpn, backbone->e_reg);
            backbone->e_reg = 0;
            break;
        case BACKBONE_KEY_CLR:
            programmer_rpn_clrni(&backbone->rpn, backbone->e_reg);
            backbone->e_reg = 0;
            break;
        case BACKBONE_KEY_ADD:
            if(backbone->shift == BACKBONE_SHIFT_UP) {
                if(backbone->e_reg) programmer_rpn_ori(&backbone->rpn, backbone->e_reg);
                else programmer_rpn_or(&backbone->rpn);
            } else {
                if(backbone->e_reg) programmer_rpn_addi(&backbone->rpn, backbone->e_reg);
                else programmer_rpn_add(&backbone->rpn);
            }
            backbone->e_reg = 0;
            break;
        case BACKBONE_KEY_SUB:
            if(backbone->shift == BACKBONE_SHIFT_UP) {
                programmer_rpn_not(&backbone->rpn);
            } else {
                if(backbone->e_reg) programmer_rpn_subi(&backbone->rpn, backbone->e_reg);
                else programmer_rpn_sub(&backbone->rpn);
            }
            backbone->e_reg = 0;
            break;
        case BACKBONE_KEY_MUL:
            if(backbone->shift == BACKBONE_SHIFT_UP) {
                if(backbone->e_reg) programmer_rpn_andi(&backbone->rpn, backbone->e_reg);
                else programmer_rpn_and(&backbone->rpn);
            } else if(backbone->shift == BACKBONE_SHIFT_DOWN) {
                if(backbone->e_reg) programmer_rpn_dwmuli(&backbone->rpn, backbone->e_reg);
                else programmer_rpn_dwmul(&backbone->rpn);
            } else {
                if(backbone->e_reg) programmer_rpn_muli(&backbone->rpn, backbone->e_reg);
                else programmer_rpn_mul(&backbone->rpn);
            }
            backbone->e_reg = 0;
            break;
        case BACKBONE_KEY_DIV:
            if(backbone->shift == BACKBONE_SHIFT_UP) {
                if(backbone->e_reg) programmer_rpn_xori(&backbone->rpn, backbone->e_reg);
                else programmer_rpn_xor(&backbone->rpn);
            } else {
                if(backbone->e_reg) programmer_rpn_divi(&backbone->rpn, backbone->e_reg);
                else programmer_rpn_div(&backbone->rpn);
            }
            backbone->e_reg = 0;
            break;
        case BACKBONE_KEY_MOD:
            if(backbone->e_reg) programmer_rpn_modi(&backbone->rpn, backbone->e_reg);
            else programmer_rpn_mod(&backbone->rpn);
            break;
        case BACKBONE_KEY_WS:
            if(backbone->shift == BACKBONE_SHIFT_UP) {
                if(backbone->e_reg == 8) programmer_rpn_set_ws(&backbone->rpn, PROGRAMMER_RPN_WS_8);
                else if(backbone->e_reg == 16) programmer_rpn_set_ws(&backbone->rpn, PROGRAMMER_RPN_WS_16);
                backbone->e_reg = 0;
            } else if(backbone->shift == BACKBONE_SHIFT_DOWN) {
                programmer_rpn_set_ws(&backbone->rpn, PROGRAMMER_RPN_WS_64);
            } else {
                programmer_rpn_set_ws(&backbone->rpn, PROGRAMMER_RPN_WS_32);
            }
            break;
        case BACKBONE_KEY_SWAP:
            if(backbone->shift == BACKBONE_SHIFT_UP) programmer_rpn_clear_all(&backbone->rpn);
            else if(backbone->shift == BACKBONE_SHIFT_DOWN) programmer_rpn_clear_X(&backbone->rpn);
            else programmer_rpn_swap_XY(&backbone->rpn);
            break;
    }
    if(consume_shift) backbone->shift = BACKBONE_SHIFT_NONE;
    backbone->frontend->frontend_refresh(backbone->e_reg, !!(backbone->e_reg), backbone->base, backbone->shift);
}
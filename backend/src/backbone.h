#ifndef BACKBONE_H
#define BACKBONE_H

#include "programmer_rpn.h"
#include "backbone_utils.h"
#include "frontend_interface.h"
#include <stdint.h>

typedef enum {
    BACKBONE_KEY_0 = 0,
    BACKBONE_KEY_1 = 1,
    BACKBONE_KEY_2 = 2,
    BACKBONE_KEY_3 = 3,
    BACKBONE_KEY_4 = 4,
    BACKBONE_KEY_5 = 5,
    BACKBONE_KEY_6 = 6,
    BACKBONE_KEY_7 = 7,
    BACKBONE_KEY_8 = 8,
    BACKBONE_KEY_9 = 9,
    BACKBONE_KEY_A = 10,
    BACKBONE_KEY_B = 11,
    BACKBONE_KEY_C = 12,
    BACKBONE_KEY_D = 13,
    BACKBONE_KEY_E = 14,
    BACKBONE_KEY_F = 15,
    BACKBONE_KEY_ESC,
    BACKBONE_KEY_LR,
    BACKBONE_KEY_RR,
    BACKBONE_KEY_BACK,
    BACKBONE_KEY_ROTATE,
    BACKBONE_KEY_LS,
    BACKBONE_KEY_RS,
    BACKBONE_KEY_MOD,
    BACKBONE_KEY_SET,
    BACKBONE_KEY_OCT,
    BACKBONE_KEY_HEX,
    BACKBONE_KEY_DIV,
    BACKBONE_KEY_CLR,
    BACKBONE_KEY_DEC,
    BACKBONE_KEY_BIN,
    BACKBONE_KEY_MUL,
    BACKBONE_KEY_UP,
    BACKBONE_KEY_WS,
    BACKBONE_KEY_ENTER,
    BACKBONE_KEY_SUB,
    BACKBONE_KEY_DOWN,
    BACKBONE_KEY_SWAP,
    BACKBONE_KEY_OVRX,
    BACKBONE_KEY_CHS,
    BACKBONE_KEY_ADD
} backbone_key_t;

typedef struct {
    programmer_rpn_t rpn;
    i_frontend_t *frontend;
    uint64_t e_reg;
    backbone_base_t base;
    backbone_shift_t shift;
} backbone_t;

void backbone_init(backbone_t *backbone, i_frontend_t *frontend);

void backbone_key(backbone_t *backbone, backbone_key_t key);

#endif
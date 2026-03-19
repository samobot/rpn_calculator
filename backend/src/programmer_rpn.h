#ifndef PROGRAMMER_RPN_H
#define PROGRAMMER_RPN_H

#include <stdint.h>

typedef enum {
    PROGRAMMER_RPN_WS_8 = 8,
    PROGRAMMER_RPN_WS_16 = 16,
    PROGRAMMER_RPN_WS_32 = 32,
    PROGRAMMER_RPN_WS_64 = 64,
} programmer_rpn_ws_t;

typedef struct {
    uint64_t X_reg;
    uint64_t Y_reg;
    uint64_t Z_reg;
    uint64_t T_reg;
    uint8_t word_size;
} programmer_rpn_t;

void programmer_rpn_init(programmer_rpn_t *s);
void programmer_rpn_push(programmer_rpn_t *s, uint64_t value);
uint64_t programmer_rpn_pop(programmer_rpn_t *s);
void programmer_rpn_set_ws(programmer_rpn_t *s, programmer_rpn_ws_t ws);
void programmer_rpn_or(programmer_rpn_t *s);
void programmer_rpn_ori(programmer_rpn_t *s, uint64_t e_reg);
void programmer_rpn_not(programmer_rpn_t *s);
void programmer_rpn_and(programmer_rpn_t *s);
void programmer_rpn_andi(programmer_rpn_t *s, uint64_t e_reg);

#endif
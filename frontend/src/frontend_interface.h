#ifndef FRONTEND_INTERFACE_H
#define FRONTEND_INTERFACE_H

#include "../../backend/src/programmer_rpn.h"
#include <stdbool.h>

typedef enum {
    FRONTEND_BASE_HEX,
    FRONTEND_BASE_BIN,
    FRONTEND_BASE_OCT,
    FRONTEND_BASE_DEC,
    FRONTEND_BASE_ASCII
} frontend_base_t;

typedef enum {
    FRONTEND_SHIFT_NONE,
    FRONTEND_SHIFT_UP,
    FRONTEND_SHIFT_DOWN
} frontend_shift_t;

typedef struct {
    void (*frontend_init)(programmer_rpn_t*);
    void (*frontend_refresh)(uint64_t, bool);
    void (*frontend_set_base)(frontend_base_t);
    frontend_base_t (*frontend_get_base)(void);
    void (*frontend_set_shift)(frontend_shift_t);
} i_frontend_t;

#endif
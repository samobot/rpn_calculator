#ifndef FRONTEND_INTERFACE_H
#define FRONTEND_INTERFACE_H

#include "programmer_rpn.h"
#include "backbone_utils.h"
#include <stdbool.h>

typedef struct {
    void (*frontend_init)(programmer_rpn_t*);
    void (*frontend_refresh)(uint64_t, bool, backbone_base_t, backbone_shift_t);
} i_frontend_t;

#endif
#ifndef BACKBONE_UTILS_H
#define BACKBONE_UTILS_H

typedef enum {
    BACKBONE_BASE_HEX = 16,
    BACKBONE_BASE_BIN = 2,
    BACKBONE_BASE_OCT = 8,
    BACKBONE_BASE_DEC = 10,
    BACKBONE_BASE_ASCII = 256
} backbone_base_t;

typedef enum {
    BACKBONE_SHIFT_NONE,
    BACKBONE_SHIFT_UP,
    BACKBONE_SHIFT_DOWN
} backbone_shift_t;

#endif
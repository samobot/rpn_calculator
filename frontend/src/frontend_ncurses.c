#include "frontend_ncurses.h"
#include "backbone_utils.h"
#include "programmer_rpn.h"
#include <ncurses.h>
#include <stdint.h>

programmer_rpn_t *programmer_rpn;
WINDOW *window;

char output_string[] = " : %  l ";
char output_string_ascii[] = " : ";

#define ALL_ONES ((uint64_t)~(0L)) // this generates all 1's

void frontend_sign_extend(uint64_t *x, programmer_rpn_ws_t ws) {
    if(*x & 1<<(ws-1)) {
        *x |= ALL_ONES << (ws-1);
    }
}

void print_ASCII(char initial, int y, int x, uint64_t reg) {
    uint8_t output[8];
    for(uint8_t i = 0; i<8; i++) {
        output[7-i] = 0xFF & (reg >> (8*i));
        if(output[7-i] == 0) output[7-i] = ' '; // convert \0 to ' ' to prevent print from terminating early
    }
    mvwprintw(window, y, x, "%c: %8s", initial, output);
}

void frontend_ncurses_refresh_screen(uint64_t e_reg, bool e_visible, backbone_base_t base, backbone_shift_t shift) {
    wclear(window);
    box(window, 0, 0);
    if(base == BACKBONE_BASE_ASCII) {
        print_ASCII('T', 1, 0, programmer_rpn->T_reg);
        print_ASCII('Z', 2, 0, programmer_rpn->Z_reg);
        print_ASCII('Y', 3, 0, programmer_rpn->Y_reg);
        print_ASCII('X', 4, 0, programmer_rpn->X_reg);
        if(e_visible) {
            print_ASCII('E', 5, 0, e_reg);
        }
        return;
    }

    switch(base) {
        case BACKBONE_BASE_HEX:
            output_string[4] = '1';
            output_string[5] = '6';
            output_string[7] = 'X';
            break;
        case BACKBONE_BASE_OCT:
            output_string[4] = '2';
            output_string[5] = '2';
            output_string[7] = 'o';
            break;
        case BACKBONE_BASE_BIN:
            output_string[4] = '6';
            output_string[5] = '4';
            output_string[7] = 'b';
            break;
        case BACKBONE_BASE_DEC:
            output_string[4] = '2';
            output_string[5] = '0';
            output_string[7] = programmer_rpn->sign_mode == PROGRAMMER_RPN_2S_COMP ? 'd' : 'u';
            break;
    }

    uint64_t T = programmer_rpn->T_reg;
    uint64_t Z = programmer_rpn->Z_reg;
    uint64_t Y = programmer_rpn->Y_reg;
    uint64_t X = programmer_rpn->X_reg;
    if(programmer_rpn->sign_mode == PROGRAMMER_RPN_2S_COMP) {
        frontend_sign_extend(&T, programmer_rpn->word_size);
        frontend_sign_extend(&Z, programmer_rpn->word_size);
        frontend_sign_extend(&Y, programmer_rpn->word_size);
        frontend_sign_extend(&X, programmer_rpn->word_size);
    }

    if(shift == BACKBONE_SHIFT_UP) mvwprintw(window, 1, 1, "/\\");
    else if(shift == BACKBONE_SHIFT_DOWN) mvwprintw(window, 1, 1, "\\/");
    if(programmer_rpn->word_size == PROGRAMMER_RPN_WS_64) mvwprintw(window, 1, 8, "64b");
    else if(programmer_rpn->word_size == PROGRAMMER_RPN_WS_32) mvwprintw(window, 1, 8, "32b");
    else if(programmer_rpn->word_size == PROGRAMMER_RPN_WS_16) mvwprintw(window, 1, 8, "16b");
    else if(programmer_rpn->word_size == PROGRAMMER_RPN_WS_8) mvwprintw(window, 1, 8, " 8b");
    output_string[0] = 'T'; mvwprintw(window, 2, 1, output_string, T);
    output_string[0] = 'Z'; mvwprintw(window, 3, 1, output_string, Z);
    output_string[0] = 'Y'; mvwprintw(window, 4, 1, output_string, Y);
    output_string[0] = 'X'; mvwprintw(window, 5, 1, output_string, X);
    if(e_visible) {
        output_string[0] = 'E';
        mvwprintw(window, 6, 1, output_string, e_reg);
    }
    wrefresh(window);
}

void frontend_ncurses_init(programmer_rpn_t *rpn) {
    programmer_rpn = rpn;

    initscr();
    noecho();
    cbreak();

    window = newwin(8, 70, 0, 0);
    box(window, 0, 0);
    wrefresh(window);
}

static i_frontend_t ncurses_frontend_interface = {
    frontend_ncurses_init,
    frontend_ncurses_refresh_screen,
};

i_frontend_t *frontend_ncurses_get_interface() {
    return &ncurses_frontend_interface;
}
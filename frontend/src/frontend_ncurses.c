#include "frontend_interface.h"
#include <stdio.h>
#include <ncurses.h>

programmer_rpn_t *programmer_rpn;
frontend_base_t base;
frontend_shift_t shift = FRONTEND_SHIFT_NONE;

char output_string[] = " : %  l ";
char output_string_ascii[] = " : ";

void print_ASCII(char initial, int y, int x, uint64_t reg) {
    uint8_t output[8];
    for(uint8_t i = 0; i<8; i++) {
        output[7-i] = 0xFF & (reg >> (8*i));
        if(output[7-i] == 0) output[7-i] = ' '; // convert \0 to ' ' to prevent print from terminating early
    }
    mvwprintw(stdscr, y, x, "%c: %8s", initial, output);
}

void refresh_screen(uint64_t e_reg, bool e_visible) {
    clear();
    if(base == FRONTEND_BASE_ASCII) {
        print_ASCII('T', 1, 0, programmer_rpn->T_reg);
        print_ASCII('Z', 2, 0, programmer_rpn->Z_reg);
        print_ASCII('Y', 3, 0, programmer_rpn->Y_reg);
        print_ASCII('X', 4, 0, programmer_rpn->X_reg);
        if(e_visible) {
            print_ASCII('E', 5, 0, e_reg);
        }
        return;
    }
    if(base == FRONTEND_BASE_DEC) output_string[7] = programmer_rpn->sign_mode == PROGRAMMER_RPN_2S_COMP ? 'd' : 'u';
    if(shift == FRONTEND_SHIFT_UP) mvwprintw(stdscr, 0, 0, "/\\");
    else if(shift == FRONTEND_SHIFT_DOWN) mvwprintw(stdscr, 0, 0, "\\/");
    output_string[0] = 'T'; mvwprintw(stdscr, 1, 0, output_string, programmer_rpn->T_reg);
    output_string[0] = 'Z'; mvwprintw(stdscr, 2, 0, output_string, programmer_rpn->Z_reg);
    output_string[0] = 'Y'; mvwprintw(stdscr, 3, 0, output_string, programmer_rpn->Y_reg);
    output_string[0] = 'X'; mvwprintw(stdscr, 4, 0, output_string, programmer_rpn->X_reg);
    if(e_visible) {
        output_string[0] = 'E';
        mvwprintw(stdscr, 5, 0, output_string, e_reg);
    }
}

void set_base(frontend_base_t newbase) {
    base = newbase;
    switch(base) {
        case FRONTEND_BASE_HEX:
            output_string[4] = '1';
            output_string[5] = '6';
            output_string[7] = 'X';
            break;
        case FRONTEND_BASE_OCT:
            output_string[4] = '2';
            output_string[5] = '2';
            output_string[7] = 'o';
            break;
        case FRONTEND_BASE_BIN:
            output_string[4] = '6';
            output_string[5] = '4';
            output_string[7] = 'b';
            break;
        case FRONTEND_BASE_DEC:
            output_string[4] = '2';
            output_string[5] = '0';
            output_string[7] = programmer_rpn->sign_mode == PROGRAMMER_RPN_2S_COMP ? 'd' : 'u';
            break;
    }
}

frontend_base_t get_base() {
    return base;
}

void set_shift(frontend_shift_t newshift) {
    shift = newshift;
}

void init(programmer_rpn_t *rpn) {
    programmer_rpn = rpn;
    set_base(FRONTEND_BASE_HEX);

    initscr();
    noecho();
    cbreak();
}

static i_frontend_t ncurses_frontend_interface = {
    init,
    refresh_screen,
    set_base,
    get_base,
    set_shift
};

i_frontend_t *frontend_ncurses_get_interface() {
    return &ncurses_frontend_interface;
}
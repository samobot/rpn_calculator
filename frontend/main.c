#include "src/frontend_interface.h"
#include "src/frontend_ncurses.h"
#include <ncurses.h>
#include <stdint.h>

i_frontend_t *frontend_interface;
programmer_rpn_t rpn;

uint64_t add_to_e(uint64_t e, uint8_t addend, frontend_base_t base) {
    switch(base) {
        case FRONTEND_BASE_BIN:
            if(addend == 0 || addend == 1) {
                return (e<<1) | addend;
            }
            break;
        case FRONTEND_BASE_OCT:
            if(addend >= 0 && addend <= 7) {
                return (e<<3) | addend;
            }
            break;
        case FRONTEND_BASE_DEC:
            if(addend >= 0 && addend <= 9) {
                return (e * 10) + addend;
            }
            break;
        case FRONTEND_BASE_HEX:
        case FRONTEND_BASE_ASCII:
            if(addend >= 0 && addend <= 15) {
                return (e << 4) | addend;
            }
            break;
    }
    return e;
}

uint64_t backspace_e(uint64_t e, frontend_base_t base) {
    switch(base) {
        case FRONTEND_BASE_BIN:
            return e>>1;
        case FRONTEND_BASE_OCT:
            return e>>3;
        case FRONTEND_BASE_DEC:
            return e/10;
        case FRONTEND_BASE_HEX:
        case FRONTEND_BASE_ASCII:
            return e>>4;
    }
}

int main() {
    programmer_rpn_init(&rpn);
    frontend_interface = frontend_ncurses_get_interface();

    frontend_interface->frontend_init(&rpn);
    rpn.X_reg = 0x796565746564;
    rpn.Y_reg = 0xBABE;
    rpn.Z_reg = ~0L;
    rpn.T_reg = 0;
    frontend_interface->frontend_set_base(FRONTEND_BASE_ASCII);
    frontend_interface->frontend_refresh(0, 1);

    uint64_t e_reg;
    frontend_shift_t shift;

    keypad(stdscr, TRUE);

    int x = 0;
    while(x != 27) {
        bool shift_set = false;
        bool retain_e = false;
        switch(x) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                e_reg = add_to_e(e_reg, x - 48, frontend_interface->frontend_get_base());
                retain_e = true;
                break;
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
                e_reg = add_to_e(e_reg, x - 87, frontend_interface->frontend_get_base());
                retain_e = true;
                break;
            case 'l':
                frontend_interface->frontend_set_base(FRONTEND_BASE_BIN);
                retain_e = true;
                break;
            case 'u':
                frontend_interface->frontend_set_base(FRONTEND_BASE_OCT);
                retain_e = true;
                break;
            case 'y':
                frontend_interface->frontend_set_base(FRONTEND_BASE_DEC);
                retain_e = true;
                break;
            case ';':
                if(shift == FRONTEND_SHIFT_UP) frontend_interface->frontend_set_base(FRONTEND_BASE_ASCII);
                else frontend_interface->frontend_set_base(FRONTEND_BASE_HEX);
                retain_e = true;
                break;
            case KEY_BACKSPACE:
                e_reg = backspace_e(e_reg, frontend_interface->frontend_get_base());
                retain_e = true;
                break;
            case 10:
            case 13:
            case KEY_ENTER:
                if(shift == FRONTEND_SHIFT_UP) programmer_rpn_override_X(&rpn, e_reg);
                else programmer_rpn_enter(&rpn, e_reg);
                break;
            case '+':
                if(e_reg) programmer_rpn_addi(&rpn, e_reg);
                else programmer_rpn_add(&rpn);
                break;
            case '-':
                if(e_reg) programmer_rpn_subi(&rpn, e_reg);
                else programmer_rpn_sub(&rpn);
                break;
            case '*':
                if(e_reg) programmer_rpn_muli(&rpn, e_reg);
                else programmer_rpn_mul(&rpn);
                break;
            case '/':
                if(e_reg) programmer_rpn_divi(&rpn, e_reg);
                else programmer_rpn_div(&rpn);
                break;
            case ',':
                if(e_reg) programmer_rpn_lshiftni(&rpn, e_reg);
                else if(shift == FRONTEND_SHIFT_UP) programmer_rpn_lshiftn(&rpn);
                else programmer_rpn_lshift(&rpn);
                break;
            case '.':
                if(e_reg) programmer_rpn_rshiftni(&rpn, e_reg);
                else if(shift == FRONTEND_SHIFT_UP) programmer_rpn_rshiftn(&rpn);
                else programmer_rpn_rshift(&rpn);
                break;
             case 'o':
                if(e_reg) programmer_rpn_lrollni(&rpn, e_reg);
                else if(shift == FRONTEND_SHIFT_UP) programmer_rpn_lrolln(&rpn);
                else programmer_rpn_lroll(&rpn);
                break;
            case '\'':
                if(e_reg) programmer_rpn_rrollni(&rpn, e_reg);
                else if(shift == FRONTEND_SHIFT_UP) programmer_rpn_rrolln(&rpn);
                else programmer_rpn_rroll(&rpn);
                break;
            case '\\':
                programmer_rpn_change_sign(&rpn);
                break;
            case KEY_UP:
                if(shift == FRONTEND_SHIFT_NONE || shift == FRONTEND_SHIFT_DOWN) { // set shift to down unless it's already down, in which case let it clear automatically
                    shift = FRONTEND_SHIFT_UP;
                    frontend_interface->frontend_set_shift(FRONTEND_SHIFT_UP);
                    shift_set = true;
                }
                retain_e = true;
                break;
            case KEY_DOWN:
                if(shift == FRONTEND_SHIFT_NONE || shift == FRONTEND_SHIFT_UP) {
                    shift = FRONTEND_SHIFT_DOWN;
                    frontend_interface->frontend_set_shift(FRONTEND_SHIFT_DOWN);
                    shift_set = true;
                }
                retain_e = true;
                break;
        }
        if(!shift_set) { // if shift was not actively set, then it must have been consumed by hitting a key, this saves clearing it on every switch case
            shift = FRONTEND_SHIFT_NONE;
            frontend_interface->frontend_set_shift(FRONTEND_SHIFT_NONE);
        }
        if(!retain_e) {
            e_reg = 0;
        }
        frontend_interface->frontend_refresh(e_reg, !!e_reg);
        x = getch();
    }
    
    endwin();

    return 0;
}
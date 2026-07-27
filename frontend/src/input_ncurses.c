#include "input_ncurses.h"
#include "backbone.h"
#include "backbone_utils.h"
#include <ncurses.h>

WINDOW *inputwin;

#define BOX_LENGTH 8
#define BOX_HEIGHT 2
#define ROWS 6
#define COLUMNS 7
#define WINDOW_LENGTH COLUMNS*BOX_LENGTH+1
#define WINDOW_HEIGHT ROWS*BOX_HEIGHT+1

const char no_shift[ROWS][COLUMNS][BOX_LENGTH] = {
    "  ESC  ", " (L)R ", " (R)R ", "   D   ", "   E   ", "   F   ", " BACK ",
    " SR \\/ ", " (<)< ", " (>)> ", "   A   ", "   B   ", "   C   ", "  MOD  ",
    " SETn ", "  OCT  ", "  HEX  ", "   7   ", "   8   ", "   9   ", "   /   ",
    " CLRn ", "  DEC  ", "  BIN  ", "   4   ", "   5   ", "   6   ", "   *   ",
    "  /\\  ", "  32b  ", " ENTER ", "   1   ", "   2   ", "   3   ", "   -   ",
    "  \\/  ", "  X~Y  ", " ", "   0   ", " OVRX ", "  CHS  ", "   +   "
};

const char up_shift[ROWS][COLUMNS][BOX_LENGTH] = {
    "", "  LLn  ", "  RRn  ", "", "", "", "",
    " SR /\\ ", "  <<n  ", "  >>n  ", "", "", "", "   &|  ",
    "", "", " ASCII ", "", "", "", "  XOR  ",
    "", "", "", "", "", "", "  AND  ",
    "  /\\  ", "  WSn  ", "", "", "  2's  ", "UNSIGN", "  NOT  ",
    "  \\/  ", "  CLA  ", "", "", "", "", "  OR  "
};

const char down_shift[ROWS][COLUMNS][BOX_LENGTH] = {
    "", "", "", "", "", "", "",
    "", "", "", "", "", "", "",
    "", "", "", "", "", "", "",
    "", "", "", "", "", "", "  DW*  ",
    "  /\\  ", "  64b  ", "", "", "", "", "",
    "  \\/  ", "  CLX  ", "", "", "", "", ""
};

backbone_key_t keymap[ROWS][COLUMNS] = {
    BACKBONE_KEY_ESC, BACKBONE_KEY_LR, BACKBONE_KEY_RR, BACKBONE_KEY_D, BACKBONE_KEY_E, BACKBONE_KEY_F, BACKBONE_KEY_BACK,
    BACKBONE_KEY_ROTATE, BACKBONE_KEY_LS, BACKBONE_KEY_RS, BACKBONE_KEY_A, BACKBONE_KEY_B, BACKBONE_KEY_C, BACKBONE_KEY_MOD,
    BACKBONE_KEY_SET, BACKBONE_KEY_OCT, BACKBONE_KEY_HEX, BACKBONE_KEY_7, BACKBONE_KEY_8, BACKBONE_KEY_9, BACKBONE_KEY_DIV,
    BACKBONE_KEY_CLR, BACKBONE_KEY_DEC, BACKBONE_KEY_BIN, BACKBONE_KEY_4, BACKBONE_KEY_5, BACKBONE_KEY_6, BACKBONE_KEY_MUL,
    BACKBONE_KEY_UP, BACKBONE_KEY_WS, BACKBONE_KEY_ENTER, BACKBONE_KEY_1, BACKBONE_KEY_2, BACKBONE_KEY_3, BACKBONE_KEY_SUB,
    BACKBONE_KEY_DOWN, BACKBONE_KEY_SWAP, 0, BACKBONE_KEY_0, BACKBONE_KEY_OVRX, BACKBONE_KEY_CHS, BACKBONE_KEY_ADD,
};

void handle_mouse(backbone_t *backbone, MEVENT *event) {
    int y = event->y;
    int x = event->x;
    wmouse_trafo(inputwin, &y, &x, false);
    if(y % BOX_HEIGHT == 0 || x % BOX_LENGTH == 0) return;
    int btnX = x / BOX_LENGTH;
    int btnY = y / BOX_HEIGHT;
    backbone_key(backbone, keymap[btnY][btnX]);
}

void draw_ui(backbone_t *backbone) {
    wclear(inputwin);
    box(inputwin, 0, 0);

    for(int i = 0; i < ROWS-1; i++) {
        mvwhline(inputwin, (BOX_HEIGHT*i)+BOX_HEIGHT, 1, 0, WINDOW_LENGTH-2);
    }
    for(int i = 0; i < COLUMNS-1; i++) {
        mvwvline(inputwin, 1, (BOX_LENGTH*i)+BOX_LENGTH, 0, WINDOW_HEIGHT-2);
    }
    for(int i = 0; i < ROWS*COLUMNS; i++) {
        int x = i % 7;
        int y = i / 7;
        if(backbone->shift == BACKBONE_SHIFT_UP) mvwprintw(inputwin, y*BOX_HEIGHT+1, x*BOX_LENGTH+1, "%s", up_shift[y][x]);
        else if(backbone->shift == BACKBONE_SHIFT_DOWN) mvwprintw(inputwin, y*BOX_HEIGHT+1, x*BOX_LENGTH+1, "%s", down_shift[y][x]);
        else mvwprintw(inputwin, y*BOX_HEIGHT+1, x*BOX_LENGTH+1, "%s", no_shift[y][x]);
    }

    wrefresh(inputwin);
}

void input_ncurses_begin(backbone_t *backbone) {
    inputwin = newwin(WINDOW_HEIGHT, WINDOW_LENGTH, 8, 0);
    keypad(inputwin, TRUE);
    mousemask(BUTTON1_CLICKED, NULL);

    int x = 0;
    MEVENT mouse_event;
    while(x != 27) {
        
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
                backbone_key(backbone, (backbone_key_t)(x - 48));
                break;
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
                backbone_key(backbone, (backbone_key_t)(x - 87));
                break;
            case KEY_UP:
                backbone_key(backbone, BACKBONE_KEY_UP);
                break;
            case KEY_DOWN:
                backbone_key(backbone, BACKBONE_KEY_DOWN);
                break;
            case KEY_ENTER:
            case '\n':
                backbone_key(backbone, BACKBONE_KEY_ENTER);
                break;
            case KEY_MOUSE:
                getmouse(&mouse_event);
                handle_mouse(backbone, &mouse_event);
                break;
        }
        draw_ui(backbone);
        x = wgetch(inputwin);
    }
    
    endwin();
}
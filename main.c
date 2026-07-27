#include "input_ncurses.h"
#include "frontend_ncurses.h"

int main() {
    
    backbone_t backbone;
    backbone_init(&backbone, frontend_ncurses_get_interface());
    input_ncurses_begin(&backbone);

    return 0;
}
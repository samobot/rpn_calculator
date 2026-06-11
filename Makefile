calc: backend/src/programmer_rpn.c frontend/src/frontend_ncurses.c frontend/main.c
	gcc -o calc backend/src/programmer_rpn.c frontend/src/frontend_ncurses.c frontend/main.c -lncurses
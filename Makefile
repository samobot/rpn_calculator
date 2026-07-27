build/calc: build backend/src/* frontend/src/* main.c
	gcc -o build/calc -lncurses -Ibackend/src -Ifrontend/src backend/src/*.c frontend/src/*.c main.c 

build:
	mkdir build
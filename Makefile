build/rpnc: build backend/src/* frontend/src/* main.c
	gcc -flto -O2 -o build/rpnc -lncurses -Ibackend/src -Ifrontend/src backend/src/*.c frontend/src/*.c main.c 

build:
	mkdir build
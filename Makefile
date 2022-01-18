.PHONY: run
run: main
	./main

main: main.c application.c nanovg/src/nanovg.c
	cc $(shell pkg-config --cflags gl glew glfw3) -o main application.c main.c ./nanovg/src/nanovg.c -lm $(shell pkg-config --libs gl glew glfw3 )

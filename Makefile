ds/libds.a:
	make -C ./ds

.PHONY: run
run: main
	./main

main: main.c application.c nanovg/src/nanovg.c ds/libds.a
	cc $(shell pkg-config --cflags gl glew glfw3) -o main application.c main.c ./ds/libds.a ./nanovg/src/nanovg.c -lm $(shell pkg-config --libs gl glew glfw3 )

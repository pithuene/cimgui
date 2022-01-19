.PHONY: all
all: main

.PHONY: run
run: main
	./main


ds/libds.a:
	make -C ./ds

events/events.a:
	$(MAKE) -C events
main: main.c application.c nanovg/src/nanovg.c ds/libds.a events/events.a
	cc $(shell pkg-config --cflags gl glew glfw3) -o main application.c main.c events/events.a ./ds/libds.a ./nanovg/src/nanovg.c -lm $(shell pkg-config --libs gl glew glfw3 )

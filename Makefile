.PHONY: all
all: main

.PHONY: run
run: main
	./main

.PHONY: clean
clean:
	make -C events clean
	make -C font clean
	make -C utils clean
	make -C widgets clean
	rm -f main

ds/libds.a:
	make -C ./ds

font/font.a: force_look
	$(MAKE) -C font

events/events.a: force_look
	$(MAKE) -C events

utils/utils.a: force_look
	$(MAKE) -C utils

widgets/widgets.a: force_look
	$(MAKE) -C widgets

main: main.c application.c nanovg/src/nanovg.c ds/libds.a events/events.a font/font.a widgets/widgets.a utils/utils.a 
	cc -std=c99 -pedantic -Wall $(shell pkg-config --cflags fontconfig gl glew glfw3) -o main application.c main.c events/events.a font/font.a widgets/widgets.a utils/utils.a ./ds/libds.a ./nanovg/src/nanovg.c -lm $(shell pkg-config --libs fontconfig gl glew glfw3 )

force_look:
	true

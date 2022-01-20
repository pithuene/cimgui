.PHONY: all
all: main

.PHONY: run
run: main
	./main

.PHONY: clean
clean:
	make -C events clean
	make -C utils clean
	make -C widgets clean
	rm -f main

ds/libds.a:
	make -C ./ds

events/events.a: force_look
	$(MAKE) -C events

utils/utils.a: force_look
	$(MAKE) -C utils

widgets/widgets.a: force_look
	$(MAKE) -C widgets

main: main.c application.c nanovg/src/nanovg.c ds/libds.a events/events.a widgets/widgets.a utils/utils.a 
	cc $(shell pkg-config --cflags gl glew glfw3) -o main application.c main.c events/events.a widgets/widgets.a utils/utils.a ./ds/libds.a ./nanovg/src/nanovg.c -lm $(shell pkg-config --libs gl glew glfw3 )

force_look:
	true

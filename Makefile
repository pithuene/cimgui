CFLAGS = -std=c99 -pedantic -Wall -Wno-override-init-side-effects -Werror 

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
	rm -f nanovg.o
	rm -f main

nanovg.o: nanovg/src/nanovg.c
	cc -c -std=c99 nanovg/src/nanovg.c

ds/libds.a:
	make -C ./ds

font/font.a: force_look
	$(MAKE) -C font PARENT_CFLAGS="$(CFLAGS)"

events/events.a: force_look
	$(MAKE) -C events PARENT_CFLAGS="$(CFLAGS)"

utils/utils.a: force_look
	$(MAKE) -C utils PARENT_CFLAGS="$(CFLAGS)"

widgets/widgets.a: force_look
	$(MAKE) -C widgets PARENT_CFLAGS="$(CFLAGS)"

main: main.c application.c nanovg.o ds/libds.a events/events.a font/font.a widgets/widgets.a utils/utils.a 
	cc $(CFLAGS) $(shell pkg-config --cflags fontconfig gl glew glfw3) -o main application.c main.c events/events.a font/font.a widgets/widgets.a utils/utils.a ./ds/libds.a nanovg.o -lm $(shell pkg-config --libs fontconfig gl glew glfw3 )

force_look:
	true

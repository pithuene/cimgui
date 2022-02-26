CFLAGS = -g -std=c99 -pedantic -Wall -Wno-override-init-side-effects -Werror 

.PHONY: all
all: main

.PHONY: run
run: render.so

.PHONY: runmain
runmain: main render.so
	./main

.PHONY: clean
clean:
	make -C events clean
	make -C font clean
	make -C widgets clean
	make -C element clean
	make -C checktag clean
	rm -f nanovg.o
	rm -f main
	rm -f render.so

nanovg.o: nanovg/src/nanovg.c
	cc -c -std=c99 nanovg/src/nanovg.c

ds/libds.a:
	make -C ./ds

font/font.a: force_look
	$(MAKE) -C font PARENT_CFLAGS="$(CFLAGS)"

events/events.a: force_look
	$(MAKE) -C events PARENT_CFLAGS="$(CFLAGS)"

widgets/widgets.a: force_look
	$(MAKE) -C widgets PARENT_CFLAGS="$(CFLAGS)"

element/element.a: force_look
	$(MAKE) -C element PARENT_CFLAGS="$(CFLAGS)"

ops/ops.a: force_look
	$(MAKE) -C ops PARENT_CFLAGS="$(CFLAGS)"

checktag/checktag.a: force_look
	$(MAKE) -C checktag PARENT_CFLAGS="$(CFLAGS)"

piecetable/piecetable.a: force_look
	$(MAKE) -C piecetable PARENT_CFLAGS="$(CFLAGS)"


main: main.c application.c nanovg.o ds/libds.a events/events.a font/font.a element/element.a ops/ops.a widgets/widgets.a checktag/checktag.a piecetable/piecetable.a render.so
	cc $(CFLAGS) $(shell pkg-config --cflags fontconfig gl glew glfw3) -o main main.c application.c events/events.a font/font.a element/element.a ops/ops.a widgets/widgets.a checktag/checktag.a piecetable/piecetable.a ./ds/libds.a nanovg.o -lm $(shell pkg-config --libs fontconfig gl glew glfw3 ) -ldl -rdynamic

render.so: render.c events/events.a font/font.a element/element.a ops/ops.a widgets/widgets.a checktag/checktag.a piecetable/piecetable.a
	cc $(CFLAGS) -shared -fPIC -o render.so render.c events/events.a font/font.a element/element.a ops/ops.a widgets/widgets.a checktag/checktag.a piecetable/piecetable.a

force_look:
	true

CFLAGS_DEBUG = -g -fprofile-arcs -ftest-coverage -fPIC
CFLAGS = -std=c99 -pedantic -Wall -Wno-override-init-side-effects -Wno-unused-function  -Werror $(CFLAGS_DEBUG)
SUB_ARCHIVES = events/events.a font/font.a element/element.a filedialog/filedialog.a ops/ops.a widgets/widgets.a checktag/checktag.a editor/editor.a editor_core/editor_core.a


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
	make -C editor_core clean
	make -C editor clean
	make -C ops clean
	make -C filedialog clean
	find . -name '*.gcda' -delete
	find . -name '*.gcno' -delete
	rm -f nanovg.o
	rm -f main
	rm -f render.so
	rm -f cimgui.a

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

filedialog/filedialog.a: force_look
	$(MAKE) -C filedialog PARENT_CFLAGS="$(CFLAGS)"

checktag/checktag.a: force_look
	$(MAKE) -C checktag PARENT_CFLAGS="$(CFLAGS)"

editor_core/editor_core.a: force_look
	$(MAKE) -C editor_core PARENT_CFLAGS="$(CFLAGS)"

editor/editor.a: force_look
	$(MAKE) -C editor PARENT_CFLAGS="$(CFLAGS)"

cimgui.a: nanovg.o ds/libds.a $(SUB_ARCHIVES)
	ar crT cimgui.a nanovg.o ds/libds.a $(SUB_ARCHIVES)

main: main.c application.c render.so cimgui.a
	cc $(CFLAGS) $(shell pkg-config --cflags fontconfig gl glew glfw3 gtk+-3.0) -o main main.c application.c cimgui.a -lm $(shell pkg-config --libs fontconfig gl glew glfw3 gtk+-3.0) -ldl -rdynamic

render.so: render.c cimgui.a
	cc $(CFLAGS) -shared -fPIC -o render.so render.c cimgui.a

.PHONY: test
test: cimgui.a clean-coverage
	$(MAKE) -C test run_test;

# Remove all previous coverage data
.PHONY: clean-coverage
clean-coverage:
	find . -name '*.gcda' -delete;
	rm -f ./coverage/*

.PHONY: coverage
coverage: test
	gcovr -s --html-details ./coverage/coverage.html --exclude-directories test -e main.c -e render.c -e nanovg/src/nanovg_gl.h;
	xdg-open ./coverage/coverage.html

force_look:
	true

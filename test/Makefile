SRCS := $(wildcard ./*.c)
OBJS := $(patsubst ./%.c,./%.o,$(SRCS))

CFLAGS = -std=c99 -pedantic -Wall -Werror 

TEST_SRCS = ../editor_core/test_editor_core.c ../font/utf8/test_utf8.c

example.o: example.c
	$(CC) $(CFLAGS) -c example.c

munit/munit.o: munit/munit.c
	$(CC) -o munit/munit.o -c ./munit/munit.c

test: example.o $(TEST_SRCS) ../cimgui.a
	$(CC) --coverage -o test example.o $(TEST_SRCS) ../cimgui.a munit/munit.o

.PHONY: run_test
run_test: test
	./test

.PHONY: clean
clean:
	rm -f $(OBJS) $(RES)

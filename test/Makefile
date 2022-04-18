SRCS := $(wildcard ./*.c)
OBJS := $(patsubst ./%.c,./%.o,$(SRCS))

CFLAGS = -std=c99 -pedantic -Wall -Werror 

example.o: example.c
	$(CC) $(CFLAGS) -c example.c

munit/munit.o: munit/munit.c
	$(CC) -o munit/munit.o -c ./munit/munit.c

test: example.o ../editor_core/editor_core.a ../font/utf8/utf8.a munit/munit.o
	$(CC) --coverage -o test example.o ../editor_core/editor_core.a ../font/utf8/utf8.a  ../ds/libds.a munit/munit.o

run_test: test
	./test

.PHONY: clean
clean:
	rm -f $(OBJS) $(RES)

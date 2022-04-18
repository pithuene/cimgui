SRCS := $(wildcard ./*.c)
OBJS := $(patsubst ./%.c,./%.o,$(SRCS))

CFLAGS = -std=c99 -pedantic -Wall -Werror 

example.o: example.c
	$(CC) $(CFLAGS) -c example.c

munit/munit.o: munit/munit.c
	$(CC) -o munit/munit.o -c ./munit/munit.c

test: example.o ../cimgui.a
	$(CC) --coverage -o test example.o ../cimgui.a munit/munit.o

run_test: test
	./test

.PHONY: clean
clean:
	rm -f $(OBJS) $(RES)

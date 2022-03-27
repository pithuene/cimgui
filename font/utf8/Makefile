SRCS := $(wildcard ./*.c)
OBJS := $(patsubst ./%.c,./%.o,$(SRCS))
RES  := utf8.a

.PHONY: all
all: $(RES)

%.o: %.c
	$(CC) $(PARENT_CFLAGS) -c $< -o $@

$(RES): $(OBJS)
	ar cr $(RES) $(OBJS)

.PHONY: clean
clean:
	rm -f $(OBJS) $(RES)
	rm -f test_utf8

test_utf8: test_utf8.c utf8.c
	$(CC) -o test_utf8 test_utf8.c utf8.c

.PHONY: run
run: test_utf8
	./test_utf8


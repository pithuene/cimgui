SRCS := $(wildcard ./*.c)
OBJS := $(patsubst ./%.c,./%.o,$(SRCS))
RES  := font.a

.PHONY: all
all: $(RES)

%.o: %.c
	$(CC) $(PARENT_CFLAGS) -c $< -o $@

$(RES): $(OBJS) utf8/utf8.a
	ar crT $(RES) $(OBJS) utf8/utf8.a

.PHONY: clean
clean:
	make -C utf8 clean
	rm -f $(OBJS) $(RES)

utf8/utf8.a: force_look
	$(MAKE) -C utf8 PARENT_CFLAGS="$(CFLAGS)"

force_look:
	true

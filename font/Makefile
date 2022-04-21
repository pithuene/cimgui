SRCS := $(wildcard ./*.c)
OBJS := $(patsubst ./%.c,./%.o,$(SRCS))
RES  := font.a

.PHONY: all
all: $(RES)

%.o: %.c
	$(CC) $(PARENT_CFLAGS) -c $< -o $@

objs_$(RES): $(OBJS)
	ar cr objs_$(RES) $(OBJS)

$(RES): objs_$(RES) utf8/utf8.a
	../merge_archives $(RES) objs_$(RES) utf8/utf8.a

.PHONY: clean
clean:
	make -C utf8 clean
	rm -f $(OBJS) $(RES)

utf8/utf8.a: force_look
	$(MAKE) -C utf8 PARENT_CFLAGS="$(PARENT_CFLAGS)"

force_look:
	true

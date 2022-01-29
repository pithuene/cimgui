SRCS := $(wildcard ./*.c)
OBJS := $(patsubst ./%.c,./%.o,$(SRCS))
RES  := font.a

.PHONY: all
all: $(RES)

%.o: %.c
	$(CC) -c $< -o $@

$(RES): $(OBJS)
	ar cr $(RES) $(OBJS)

.PHONY: clean
clean:
	rm -f $(OBJS) $(RES)

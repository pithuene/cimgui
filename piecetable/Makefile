SRCS := $(wildcard ./*.c)
OBJS := $(patsubst ./%.c,./%.o,$(SRCS))
RES  := piecetable.a

.PHONY: all
all: $(RES)

%.o: %.c
	$(CC) $(PARENT_CFLAGS) -fPIC -c $< -o $@

$(RES): $(OBJS)
	ar crT $(RES) $(OBJS)

.PHONY: clean
clean:
	rm -f $(OBJS) $(RES)

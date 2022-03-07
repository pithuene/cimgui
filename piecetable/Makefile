SRCS := $(wildcard ./*.c)
OBJS := $(patsubst ./%.c,./%.o,$(SRCS))
RES  := piecetable.a

.PHONY: all
all: $(RES)

%.o: %.c
	$(CC) $(PARENT_CFLAGS) -fPIC -c $< -o $@

$(RES): $(OBJS) md4c.o
	ar cr $(RES) $(OBJS) md4c.o

.PHONY: clean
clean:
	rm -f $(OBJS) $(RES)

md4c.o: md4c/src/md4c.c
	$(CC) -fPIC -c $< -o $@
	

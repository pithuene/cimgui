SRCS := $(wildcard ./*.c)
OBJS := $(patsubst ./%.c,./%.o,$(SRCS))
RES  := filedialog.a

.PHONY: all
all: $(RES)

%.o: %.c
	$(CC) $(PARENT_CFLAGS) -c $< -o $@

$(RES): $(OBJS) nativefiledialog/build/lib/Release/x64/libnfd.a
	ar crT $(RES) $(OBJS) nativefiledialog/build/lib/Release/x64/libnfd.a

.PHONY: clean
clean:
	$(MAKE) -C nativefiledialog/build/gmake_linux clean
	rm -f $(OBJS) $(RES)

nativefiledialog/build/lib/Release/x64/libnfd.a: force_look
	$(MAKE) -C nativefiledialog/build/gmake_linux

force_look:
	true

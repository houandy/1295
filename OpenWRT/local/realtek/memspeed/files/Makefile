CC := aarch64-linux-gcc
CFLAGS = -g -O3 -Wall -fno-tree-vectorize
ASFLAGS = $(CFLAGS)
LDFLAGS = -static-libgcc

ALL = memspeed_a53 # memspeed_a55

all: $(ALL)

memspeed_a55:    CFLAGS  += -mtune=cortex-a55
memspeed_a53:    CFLAGS  += -mtune=cortex-a53 

memspeed_%.o: memspeed.c
	$(CC) -o $@ $(CFLAGS) -c $^

memspeed_%: memspeed_%.o
	$(CC) -o $@ $(LDFLAGS)  $^

clean:
	rm -f $(ALL) *.o

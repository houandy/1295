RM=/bin/rm
CC=gcc

CFLAGS?=-Wall -Werror

all: layout-checker

layout-checker:
	$(CC) -o $@ layout-checker.c $(CFLAGS)

clean:
	$(RM) -rf layout-checker


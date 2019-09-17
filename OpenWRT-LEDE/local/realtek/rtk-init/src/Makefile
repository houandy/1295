all: init

init: rtk-init.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
	$(STRIP) $@

clean:
	rm -f *.o init

.PHONY: all clean

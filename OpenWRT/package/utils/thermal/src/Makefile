all: thermal
LDFLAGS := $(LDFLAGS)

thermal:
	$(CC) $(CFLAGS) -lpthread -lm -o $@ thermal.c 

clean:
	rm -f thermal

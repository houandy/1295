all: fan
LDFLAGS := $(LDFLAGS)

fan:
	$(CC) $(CFLAGS) -o $@ fan.c -lpthread -lm $(LDFLAGS) 

clean:
	rm -f fan

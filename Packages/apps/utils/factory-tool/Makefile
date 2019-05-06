CC=
AR=
RANLIB=
STRIP=
# INCS = -I./ -I./utils
# CFLAGS =  -g -Os -march=armv8-a -Wall
CFLAGS=

OBJS= rtk_common.o rtk_factory.o rtk_mtd.o rtk_parameter.o

#LIBS += -Wl,--start-group -L./lib/ -lMCP -lion -lpthread -lefuse -Wl,--end-group
LIBS += libfactory.a

all: install

install: factory.c utils/timeutils.c utils/alarm.c libfactory.a
	$(CC) $(CFLAGS) -c utils/timeutils.c -include config.h
	$(CC) $(CFLAGS) -c utils/alarm.c -include config.h
	$(CC) $(CFLAGS) -c factory.c -include config.h
	$(CC) -o factory $(CFLAGS) factory.o timeutils.o alarm.o $(LIBS) $(LDFLAGS)

libfactory.a: $(OBJS)
	$(AR) cr libfactory.a $(OBJS)
	$(RANLIB) libfactory.a

clean:
	rm -rf $(OBJS) *.a *.o *.bak factory

%.o: %.c
	$(CC) -c $< $(CFLAGS) -o $@

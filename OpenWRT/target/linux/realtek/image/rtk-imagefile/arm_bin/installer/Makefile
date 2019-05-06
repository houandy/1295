TOP:=`pwd`
CHIP?=rtd161x
#TOOLCHAIN_PATH ?= ~/toolchain/asdk-4.9.3-a53-EL-3.10-g2.19-a32nt-150615/
CROSS_COMPILER ?= arm-linux-gnueabihf-
RM=/bin/rm -f
CP=/bin/cp -f
CC= $(CROSS_COMPILER)gcc 
AR= $(CROSS_COMPILER)ar cr
RANLIB=$(CROSS_COMPILER)ranlib
STRIP=$(CROSS_COMPILER)strip



CFLAGS?=$(INCS)  -g -Os -Wall

OBJS= $(patsubst %.c,%.o,$(SRC)) 
LIBS=

%.o: %.cpp
	$(CC) -c $< $(CFLAGS) -o $@

%.o: %.c
	$(CC) -c $< $(CFLAGS) -o $@


all: installer

installer:
	$(CC) -o $@   installer.c rtk_common.c factory.c $(CFLAGS)  $(LIBS) -static
	$(STRIP) $@
	

clean:
	$(RM) -rf $(OBJS) *.o installer

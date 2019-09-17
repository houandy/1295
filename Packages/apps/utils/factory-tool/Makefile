# TOOLCHAIN_PATH = /PATH-TO-STAGING_DIR/staging_dir/toolchain-aarch64_cortex-a55_gcc-8.2.0_glibc
# CROSS_COMPILER = aarch64-openwrt-linux-gnu
TOOLCHAIN_PATH=
CROSS_COMPILER=

CC=$(TOOLCHAIN_PATH)/bin/$(CROSS_COMPILER)-gcc
AR=$(TOOLCHAIN_PATH)/bin/$(CROSS_COMPILER)-ar cr
RANLIB=$(TOOLCHAIN_PATH)/bin/$(CROSS_COMPILER)-ranlib
STRIP=$(TOOLCHAIN_PATH)/bin/$(CROSS_COMPILER)-strip

# Options for STORAGE_TYPE
# * emmc
#   STORAGE_TYPE=STOR_TYPE_EMMC
# * spi
#   STORAGE_TYPE=STOR_TYPE_SPI
# * nand
#   STORAGE_TYPE=STOR_TYPE_NAND
STORAGE_TYPE=

# Options for Chip_TYPE_NAME
# * rtd1619
#   CHIP_TYPE_NAME=CHIP_TYPE_THOR
# * rtd129x
#   CHIP_TYPE_NAME=CHIP_TYPE_KYLIN
CHIP_TYPE_NAME=

OBJS= rtk_common.o rtk_factory.o rtk_mtd.o rtk_parameter.o

#LIBS += -Wl,--start-group -L./lib/ -lMCP -lion -lpthread -lefuse -Wl,--end-group

INCS = -I./ -I./utils
LDFLAGS= -static
LIBS= -lc -lgcc_eh libfactory.a
DEFINES= -DNAS_ENABLE -DSYNO -D$(STORAGE_TYPE) -D$(CHIP_TYPE_NAME)
CFLAGS = -g -Os -march=armv8-a -Wall $(INCS) $(DEFINES)

all: install

install: factory.c utils/timeutils.c utils/alarm.c libfactory.a
	$(CC) $(CFLAGS) -c utils/timeutils.c -include config.h
	$(CC) $(CFLAGS) -c utils/alarm.c -include config.h
	$(CC) $(CFLAGS) -c factory.c -include config.h
	$(CC) -o factory $(CFLAGS) factory.o timeutils.o alarm.o $(LIBS) $(LDFLAGS)

libfactory.a: $(OBJS)
	$(AR) libfactory.a $(OBJS)
	$(RANLIB) libfactory.a

clean:
	rm -rf $(OBJS) *.a *.o *.bak factory

%.o: %.c
	$(CC) -c $< $(CFLAGS) -o $@

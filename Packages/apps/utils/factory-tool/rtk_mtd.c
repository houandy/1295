#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
//#include <mtd/mtd-user.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
//#include <rtk_imgdesc.h>
//#include <rtk_fwdesc.h>
#include "rtk_mtd.h"
#include "rtk_layout.h"


#define _128K_BYTE	128*1024U
#define _512K_BYTE	512*1024U
#define   _2M_BYTE	2*1024*1024U
#define   _4M_BYTE	4*1024*1024U
#define   _8M_BYTE	8*1024*1024U

#define DEFAULT_NAND_ERASESIZE 0x20000

static char MTD_CHAR_DEV_PATH[][20]=
{
    "/dev/mtd/disc",
    "/dev/mtd/mtddisc",
    "/dev/mtddisc",
    "/dev/mtd0",
    "/dev/mtd2",
    "/dev/mtd/mtd0",
    "/dev/mtd0ro",
    "/dev/mtd/0"
};
static char MTD_BLOCK_DEV_PATH[][50]=
{
    "/dev/mmcblk0",			// for linux
    "/dev/mmcblk1",
    "/dev/block/mmcblk0",	// for android
    "/dev/block/mmcblk1",
    "/dev/mtdblock4",
    "/dev/block/mtdblockdisc",
    "/dev/mtdblockdisc",
    "/dev/mtdblock/mtdblockdisc",
    "/dev/mtdblock/disc",
    "/dev/mtdblock0",
    "/dev/block/mtdblock0",
    "/dev/mtdblock/0",
    "/dev/mtdblock/mtdblock0"
};
static char* pblock_name=NULL;
static char* pchar_name=NULL;

/* Get basic MTD characteristics info (better to use sysfs) */
#define MEMGETINFO       _IOR('M', 1, struct mtd_info_user)
#define MEMGETINFO64     _IOR('M', 1, struct mtd_info_user64)
/* Erase segment of MTD */
#define MEMERASE         _IOW('M', 2, struct erase_info_user)
/* Unlock a chip (for MTD that supports it) */
#define MEMUNLOCK	 _IOW('M', 6, struct erase_info_user)

#if defined(NAS_ENABLE) && !defined(SYNO)
int rtk_open_last_mtd(const char* prefix, char** mtd_name){
   char *name;
   int last_fd = -1, fd, i;

   name = (char*)malloc(50);
   memset(name, 0, 50);

   for(i = 0; ; i++){
       sprintf(name, "%s%d", prefix, i);
       /* Test with read-only access */
       fd = open(name, O_RDONLY);
       if(-1 == fd) break;
       if(last_fd >= 0){
          close(last_fd);
       }
       last_fd = fd;
   }
   if(last_fd >= 0){
      sprintf(name, "%s%d", prefix, i-1);
      *mtd_name = name;

      fd = open(name, O_RDWR);
      close(last_fd);
      last_fd = fd;
   }
   return last_fd;
}
#endif

int rtk_open_mtd_char(char** ppstr)
{
    uint32_t i;
    int dev_fd;
#if defined(NAS_ENABLE) && !defined(SYNO)
   char *mtd_name;
   dev_fd = rtk_open_last_mtd("/dev/mtd", &mtd_name);
   if(dev_fd >= 0){
       //install_info("[NAS]Open MTD_CHAR %s\r\n", mtd_name);
       if(NULL != ppstr) *ppstr = mtd_name;
       pchar_name = mtd_name;
       return dev_fd;
   }
#endif
    for(i = 0; i < (sizeof(MTD_CHAR_DEV_PATH)/sizeof(MTD_CHAR_DEV_PATH[0])); i++)
    {
        dev_fd = open(MTD_CHAR_DEV_PATH[i], O_RDWR);
        if(-1!=dev_fd) break;
    }
    if(dev_fd < 0)
    {
        install_fail("open mtd char fail\r\n");
        return -1;
    }
    // install_info("Open MTD_CHAR %s\r\n", MTD_CHAR_DEV_PATH[i]);
    if(NULL != ppstr) *ppstr = MTD_CHAR_DEV_PATH[i];
    pchar_name = MTD_CHAR_DEV_PATH[i];
    return dev_fd;
}



int rtk_open_mtd_block(char** ppstr)
{
    uint32_t i;
    int dev_fd=-1;

#if defined(NAS_ENABLE) && !defined(SYNO)
   char *mtd_name;
   dev_fd = rtk_open_last_mtd("/dev/mtdblock", &mtd_name);
   if(dev_fd >= 0){
       //install_info("[NAS]Open MTD_BLOCK %s\r\n", mtd_name);
       if(NULL != ppstr) *ppstr = mtd_name;
       pblock_name = mtd_name;
       return dev_fd;
   }
#endif

    i = 0;
    do
    {
        for(; i<sizeof(MTD_BLOCK_DEV_PATH)/sizeof(MTD_BLOCK_DEV_PATH[0]); i++)
        {
            dev_fd = open(MTD_BLOCK_DEV_PATH[i], O_RDWR|O_SYNC);
            if(-1!=dev_fd) break;
        }
        if(dev_fd < 0)
        {
            install_debug("open mtd block fail\r\n");
            return -1;
        }
        //install_info("Open MTD_BLOCK %s\r\n", MTD_BLOCK_DEV_PATH[i]);

        // device path for SD card is /dev/mmcblkX, same as MMC.
        // check if it is MMC or SD card..
        const char *blkptr = strstr(MTD_BLOCK_DEV_PATH[i], "mmcblk");
        if( blkptr )
        {
            int fd;
            char path[64], buf[16];
            memset( path, 0, sizeof(path) );
            sprintf(path, "/sys/block/mmcblk%c/device/type", *(blkptr+strlen("mmcblk")));
            install_debug("path(%s)\n", path);
            fd = open(path, O_RDONLY);
            if( fd != -1 )
            {
                memset(buf, 0, sizeof(buf));
                read(fd, buf, sizeof(buf) );
                close(fd);
                install_debug("path(%s) string(%s)\n", path, buf);
                if( strncmp(buf, "MMC",strlen("MMC"))==0 )
                {
                    break;
                }
                else
                {
                    close(dev_fd);
                    i++;
                }
            }
            else
            {
                install_debug("open path(%s) failed\n", path);
                close(dev_fd);
                i++;
            }
        }
        else
        {
            break;
        }
    }
    while( i < (int)(sizeof(MTD_BLOCK_DEV_PATH)/sizeof(MTD_BLOCK_DEV_PATH[0]))  );

    if( i < (int)(sizeof(MTD_BLOCK_DEV_PATH)/sizeof(MTD_BLOCK_DEV_PATH[0])) )
    {
        if(NULL!=ppstr) *ppstr = MTD_BLOCK_DEV_PATH[i];
        pblock_name = MTD_BLOCK_DEV_PATH[i];
        return dev_fd;
    }

    return -1;
}
int get_mtd_block_name(char** ppstr)
{
    int ret;
    char* p;
    if(pblock_name != NULL)
    {
        *ppstr = pblock_name;
        return 0;
    }
    ret = rtk_open_mtd_block(&p);
    if(ret < 0)
    {
        return -1;
    }
    close(ret);
    *ppstr = p;
    return 0;
}

char* get_mtd_block_name_str(void)
{
    int ret;
    char* p;
    ret = rtk_open_mtd_block(&p);
    if(ret < 0)
    {
        return NULL;
    }
    close(ret);
    return p;
}

char *get_mtd_char_name(void)
{
    static char no_device[] = "NO_device";
    if(pchar_name==NULL) {
        return no_device;
    }
    return pchar_name;
}
int rtk_open_mtd_block_with_offset(unsigned int offset)
{
    uint32_t i;
    int dev_fd;
    int ret;
    for(i=0; i<sizeof(MTD_BLOCK_DEV_PATH)/sizeof(MTD_BLOCK_DEV_PATH[0]); i++)
    {
        dev_fd = open(MTD_BLOCK_DEV_PATH[i], O_RDWR|O_SYNC);
        if(dev_fd != -1) break;
    }
    if(dev_fd < 0)
    {
        install_debug("open mtd block fail\r\n");
        return -1;
    }
    pblock_name = MTD_BLOCK_DEV_PATH[i];
    //install_info("Open MTD_BLOCK %s with %u(0x%08X) offset\r\n", MTD_BLOCK_DEV_PATH[i], offset, offset);
    ret = lseek(dev_fd, offset, SEEK_SET);
    if(ret < 0)
    {
        install_debug("lseek mtd block fail\r\n");
        return -1;
    }

    return dev_fd;
}

int rtk_mtd_erase(int fd, int start, int len)
{

    struct erase_info_user erase_u;
    erase_u.start =  start;
    erase_u.length = len;

    if (ioctl(fd, MEMERASE, &erase_u))
    {
        return -1;
    }
    return 0;
}
int rtk_mtd_char_erase(unsigned int address, unsigned int size)
{

    int dev_fd;
    struct erase_info_user erase;
    dev_fd = open(get_mtd_char_name(), O_RDWR | O_SYNC);
    if(-1 == dev_fd)
    {
        install_fail("open %s fail\n", get_mtd_char_name());
        return -1;
    }
    erase.start = address;
    erase.length = size;

    if (ioctl(dev_fd, MEMERASE, &erase) != 0)
    {
        install_fail("erase fail %x %x\n", address, size);
        close(dev_fd);
        return -1;
    }

    close(dev_fd);

    return 0;
}

int rtk_mtd_char_program(unsigned char *buf, unsigned int address, unsigned size)
{


    int dev_fd;
    int ret = 0;
    dev_fd = open(get_mtd_char_name(), O_RDWR | O_SYNC);

    if(-1 == dev_fd)
    {
        install_fail("open %s fail\n", get_mtd_char_name());
        return -1;
    }

    if (lseek(dev_fd, address, SEEK_SET) != (off_t)address )
    {
        ret = -1;
        goto end_of_program;
    };
//    system("echo 3 > /sys/realtek_boards/rtice_enable");
    if (write(dev_fd, buf, size) != (ssize_t) size)
    {
        install_fail("write fail %x %x\n", address, size);
        ret = -1;
        goto end_of_program;
    }

end_of_program:
//	system("echo 0 > /sys/realtek_boards/rtice_enable");
    close(dev_fd);
    return ret;

}


int rtk_mtd_char_verify(unsigned char *buf, unsigned int address, unsigned size)
{


    int dev_fd;
    unsigned char tmp[512];
    int ret = 0;
    uint32_t count;
    uint32_t data_size, data_size_read;
    dev_fd = open(get_mtd_char_name(), O_RDWR | O_SYNC);

    if(-1 == dev_fd)
    {
        install_fail("open %s fail\n", get_mtd_char_name());
        return -1;
    }

    if (lseek(dev_fd, address, SEEK_SET) != (off_t)address)
    {
        ret = -1;
        goto end_of_verify;
    }
    for (count = 0; count < size; )
    {
        if ((size - count) < sizeof(tmp))
        {
            data_size = size - count;
        }
        else
        {
            data_size = sizeof(tmp);
        }
//		system("echo 3 > /sys/realtek_boards/rtice_enable");
        if ((data_size_read=read(dev_fd, tmp, data_size)) <= 0)
        {
            install_fail("read fail %x %x\n", address, size);
            ret = -1;
//			system("echo 0 > /sys/realtek_boards/rtice_enable");
            goto end_of_verify;
        }
        if (data_size_read != data_size)
        {
            install_fail("data_size is not equal %x %x\n", data_size_read, data_size);
            ret = -1;
            goto end_of_verify;
        }
        if (memcmp(tmp, buf, data_size))
        {
            install_fail("compare fail \n");
            ret = -1;
            goto end_of_verify;
        }
        count += data_size;
    }

end_of_verify:
//	system("echo 0 > /sys/realtek_boards/rtice_enable");
    close(dev_fd);

    return ret;

}

int rtk_get_meminfo(struct mtd_info_user* meminfo)
{
    int dev_fd;
    dev_fd = rtk_open_mtd_char(NULL);
    if(-1 == dev_fd)
        return -1;
    /* Get MTD device capability structure */
    if (ioctl(dev_fd, MEMGETINFO, meminfo))
    {
        close(dev_fd);
        return -1;
    }

    close(dev_fd);
    return 0;
}
uint32_t rtk_get_erasesize(void)
{
    int ret;
    struct mtd_info_user meminfo;
    ret = rtk_get_meminfo(&meminfo);
    if(ret < 0)
    {
        install_debug("rtk_get_meminfo fail\r\n");
        return 0;
    }
    //install_info("erasesize=0x%08x\r\n", meminfo.erasesize);
    return meminfo.erasesize;
}

int modify_addr_signature(unsigned int startAddress, unsigned int reserved_boot_size)
{
    int dev_fd;
    int ret;
    dev_fd = rtk_open_mtd_block_with_offset(reserved_boot_size);
    if(dev_fd < 0)
    {
        install_debug("rtk_open_mtd_block_with_offset() fail\r\n");
        return -1;
    }
    ret = write(dev_fd, "IMG_", 4);
    if(ret != 4)
    {
        install_debug("write signature fail\r\n");
        close(dev_fd);
        return -1;
    }
    ret = write(dev_fd, (const void *) &startAddress, 4);
    if(ret != 4)
    {
        install_debug("modify signature's num fail\r\n");
        close(dev_fd);
        return -1;
    }
    close(dev_fd);
    return 0;
}

int modify_signature(unsigned int reserved_boot_size, unsigned int flash_type)
{
    char* block_path;
    int ret;
    char command[128];
    ret = get_mtd_block_name(&block_path);
    if(ret < 0)
    {
        install_debug("get_mtd_block_name fail\r\n");
        return -1;
    }
    if(flash_type == MTD_NANDFLASH)
    {
        // modify the signature (first 8 bytes of boot table)
        sprintf(command, "echo -n RESCUE__ | dd of=%s bs=1 seek=%u", block_path, reserved_boot_size);
        ret = rtk_command(command, __LINE__, __FILE__);
    }
    else if(flash_type == MTD_NORFLASH || flash_type == MTD_DATAFLASH)
    {
        // modify the signature (first 8 bytes of boot table)
        sprintf(command, "echo -n RESCUE__ | dd of=%s bs=1 seek=0", block_path);
        ret = rtk_command(command, __LINE__, __FILE__);
    }
    else
    {
        install_debug("Unknown MTD TYPE\r\n");
        return -1;
    }
    return 0;
}

inline unsigned long long SIZE_ALIGN_BOUNDARY_MORE(unsigned long long len, unsigned long long size)
{
    return (((len - 1) & ~((size) - 1)) + size);
}

unsigned int get_nand_factory_start_addr(unsigned int* factory_start_ptr, unsigned int *erase_size_ptr, unsigned int *factory_size_ptr)
{
    unsigned int erase_size = *erase_size_ptr;
    unsigned int factory_start = *factory_start_ptr;
    unsigned int factory_size = *factory_size_ptr;
    unsigned int rsv_size = 0;
    unsigned int ret = 0;

    switch(factory_start)
    {
        /* KYLIN NAND (NAS) */
        case 0x4C0000:
            /* CONFIG_SYS_NO_BL31 */
            rsv_size = erase_size * (6+4+4);
            rsv_size += SIZE_ALIGN_BOUNDARY_MORE(0xC0000, erase_size)*4;
            rsv_size += SIZE_ALIGN_BOUNDARY_MORE(factory_size, erase_size);
            ret = rsv_size - factory_size;
            break;

        /* KYLIN NAND */
        case 0x940000:
            rsv_size = erase_size * (6+4+4+4+4+4);
            rsv_size += SIZE_ALIGN_BOUNDARY_MORE(0xC0000, erase_size)*(4+4);
            rsv_size += SIZE_ALIGN_BOUNDARY_MORE(factory_size, erase_size);
            ret = rsv_size - factory_size;
            break;

        /* THOR NAND */
        case 0xE60000:
            if (erase_size == 0x40000)
                ret = erase_size * 70;
            else
                ret = erase_size * 115;
            break;

        default:
            break;
    }

    return ret;
}

int get_flsh_info(BOOTTYPE* flash_type, unsigned int* factory_start, unsigned int* factory_size, unsigned int* erasesize)
{
    char* dev_path;
    int dev_fd;
    int ret;
    struct mtd_info_user64 meminfo;
    struct mtd_info_user meminfo32;

    //open mtd block device

    dev_fd = rtk_open_mtd_block(&dev_path);
    if(-1 == dev_fd)
    {
        return -1;
    }
    close(dev_fd);
    if (strstr(dev_path, "mmcblk"))
    {
        *flash_type = BOOT_EMMC;
        *factory_start = FACTORY_START_ADDR;
        *factory_size = FACTORY_SIZE;
        *erasesize = 512;
        return 0;
    }
    else
    {
        //open mtd char device
        dev_fd = rtk_open_mtd_char(&dev_path);
        if(-1 == dev_fd)
        {
            return -1;
        }
        /* Get MTD device capability structure */
        memset(&meminfo, 0, sizeof(struct mtd_info_user64));
        memset(&meminfo32, 0, sizeof(struct mtd_info_user));
        ret = ioctl(dev_fd, MEMGETINFO, &meminfo32);
        if (ret != 0 && 25 == errno ) {
            /* Argument length mismatch in ioctl will result in command mismatch
             * in mtdchar which returns -ENOTTY(-25)*/
            install_debug("Retry mtd info with old format\r\n");
            ret = ioctl(dev_fd, MEMGETINFO64, &meminfo);
        }
        else if (ret == 0) {
            meminfo.type = meminfo32.type;
            meminfo.flags = meminfo32.flags;
            meminfo.size = meminfo32.size;
            meminfo.erasesize = meminfo32.erasesize;
            meminfo.oobblock= meminfo32.writesize;
            meminfo.oobsize= meminfo32.oobsize;
        }

        if (ret == 0)
        {
            // flash info
            if (meminfo.type == MTD_NANDFLASH)
            {
                *flash_type = BOOT_NAND;
                *factory_start = FACTORY_START_ADDR;
                *factory_size = FACTORY_SIZE;
                *erasesize = meminfo.erasesize;
                if (meminfo.erasesize != DEFAULT_NAND_ERASESIZE)
                    *factory_start = get_nand_factory_start_addr(factory_start, erasesize, factory_size);
            }
            else if(meminfo.type == MTD_NORFLASH)
            {
                *flash_type = BOOT_SPI;
            }
#ifdef NAS_ENABLE
            else if(meminfo.type == MTD_DATAFLASH)
            {
                /* 8MB SPI NOR flash layout */
                *flash_type = BOOT_SPI;
                *erasesize = meminfo.erasesize;
                *factory_start = FACTORY_START_ADDR;
                *factory_size = FACTORY_SIZE;
            }
#endif
            close(dev_fd);
        }
        else
        {
            install_fail("Get flash info error!, errno(%d)[%s]\r\n", errno, strerror(errno));
            close(dev_fd);
            return -1;
        }
        return 0;
    }
}

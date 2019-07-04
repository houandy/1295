#ifndef __LINUX_RTK_SFC_DASH_H
#define __LINUX_RTK_SFC_DASH_H
#include <linux/spi/spi-rtk.h>
#define RDID_MANUFACTURER_ID_MASK   0x000000FF
#define RDID_DEVICE_ID_1_MASK       0x0000FF00
#define RDID_DEVICE_ID_2_MASK       0x00FF0000
#define RDID_DEVICE_EID_1_MASK      0x000000FF
#define RDID_DEVICE_EID_2_MASK      0x0000FF00

#define RDID_MANUFACTURER_ID(id)    (id & RDID_MANUFACTURER_ID_MASK)
#define RDID_DEVICE_ID_1(id)        ((id & RDID_DEVICE_ID_1_MASK) >> 8)
#define RDID_DEVICE_ID_2(id)        ((id & RDID_DEVICE_ID_2_MASK) >> 16)
#define RDID_DEVICE_EID_1(id)       (id & RDID_DEVICE_EID_1_MASK)
#define RDID_DEVICE_EID_2(id)       ((id & RDID_DEVICE_EID_2_MASK) >> 8)

#define RTK_SFC_ATTR_NONE                       0x00
#define RTK_SFC_ATTR_SUPPORT_MD_PP              0x01
#define RTK_SFC_ATTR_SUPPORT_DUAL_IO            0x02
#define RTK_SFC_ATTR_SUPPORT_DUAL_O             0x04
#define RTK_SFC_ATTR_SUPPORT_4BYTE_ADDR_MODE    0x80

#define SFC_4KB_ERASE \
{\
    sfc_info->attr      |= RTK_SFC_ATTR_SUPPORT_MD_PP;\
    sfc_info->erase_size    = 0x1000;\
    sfc_info->erase_opcode  = 0x00000020;\
}

#define SFC_64KB_ERASE \
{\
    sfc_info->attr      |= RTK_SFC_ATTR_SUPPORT_MD_PP;\
    sfc_info->erase_size    = 0x10000;\
    sfc_info->erase_opcode  = 0x000000d8;\
}

#define SFC_256KB_ERASE \
{\
    sfc_info->attr      |= RTK_SFC_ATTR_SUPPORT_MD_PP;\
    sfc_info->erase_size    = 0x40000;\
    sfc_info->erase_opcode  = 0x000000d8;\
}

#define SUPPORTED       1
#define NOT_SUPPORTED   0

typedef struct rtk_sfc_info {
    //struct semaphore rtk_sfc_lock;
    u8 manufacturer_id;
    u8 device_id1;
    u8 device_id2;
    u8 attr;
    u32 erase_size;
    u32 erase_opcode;
    u8 sec_256k_en; //256KB size erase support
    u8 sec_64k_en; //64KB size erase support
    u8 sec_32k_en; //32KB size erase support
    u8 sec_4k_en; //4KB size erase support
    struct mtd_info *mtd_info;
    unsigned char * flash_base;
    u32 flash_size;
    struct rtk_spi sfc;
    struct resource res[1];
    struct device_node *np;
    struct platform_device *dev;
} rtk_sfc_info_t;

#define MANUFACTURER_ID_SPANSION    0x01
#define MANUFACTURER_ID_STM         0x20
#define MANUFACTURER_ID_PMC         0x7f
#define MANUFACTURER_ID_SST         0xbf
#define MANUFACTURER_ID_MXIC        0xc2
#define MANUFACTURER_ID_EON         0x1c
#define MANUFACTURER_ID_ATMEL       0x1f
#define MANUFACTURER_ID_WINBOND     0xef //add by alexchang
#define MANUFACTURER_ID_ESMT        0x8c //add by alexchang
#define MANUFACTURER_ID_GD          0xc8 //add by alexchang
/*--------------------------------------------------------------------------------
  GD serial flash information list
  [GD 25Q16B] 16Mbit
  erase size: 32KB / 64KB

  [GD 25Q64B] 64Mbit
  erase size: 32KB / 64KB

  [GD 25Q128B] 128Mbit
  erase size: 32KB / 64KB
  --------------------------------------------------------------------------------*/
static int gd_init(rtk_sfc_info_t *sfc_info) {
    switch(sfc_info->device_id1) {
    case 0x40:
        switch(sfc_info->device_id2) {
        case 0x14:
            printk(KERN_NOTICE "RtkSFC MTD: GD 25Q08B detected.\n");
            SFC_4KB_ERASE;
            sfc_info->sec_64k_en = sfc_info->sec_32k_en = sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x00100000;
            break;
        case 0x15:
            printk(KERN_NOTICE "RtkSFC MTD: GD 25Q16B detected.\n");
            SFC_4KB_ERASE;
            sfc_info->sec_64k_en = sfc_info->sec_32k_en = sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x200000;
            break;
        case 0x16:
            printk(KERN_NOTICE "RtkSFC MTD: GD 25Q32B detected.\n");
            SFC_4KB_ERASE;
            sfc_info->sec_64k_en = sfc_info->sec_32k_en = sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x400000;
            break;
        case 0x17:
            printk(KERN_NOTICE "RtkSFC MTD: GD 25Q64B detected.\n");
            SFC_4KB_ERASE;
            sfc_info->sec_64k_en = sfc_info->sec_32k_en = sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x800000;
            break;
        case 0x18:
            printk(KERN_NOTICE "RtkSFC MTD: GD 25Q128B detected.\n");
            SFC_4KB_ERASE;
            sfc_info->sec_64k_en = sfc_info->sec_32k_en = sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x1000000;
            break;
        default:
            printk(KERN_NOTICE "RtkSFC MTD: GD unknown id2=0x%x detected.\n",
                   sfc_info->device_id2);
            break;
        }
        break;
    default:
        printk(KERN_NOTICE "RtkSFC MTD: GD unknown id1=0x%x detected.\n",
               sfc_info->device_id1);
        break;
    }
    if(sfc_info->erase_opcode==0xFFFFFFFF)//Set to default.
    {
        SFC_4KB_ERASE;
    }
    return 0;
}

/*--------------------------------------------------------------------------------
  SST serial flash information list
  [SST 25VF016B] 16Mbit
  erase size: 4KB / 32KB / 64KB

  [SST 25VF040B] 4Mbit
  erase size: 4KB / 32KB / 64KB
  --------------------------------------------------------------------------------*/
static int sst_init(rtk_sfc_info_t *sfc_info) {
    switch(sfc_info->device_id1) {
    case 0x25:
        switch(sfc_info->device_id2) {
        case 0x41:
            printk(KERN_NOTICE "RtkSFC MTD: SST 25VF016B detected.\n");
            sfc_info->sec_64k_en = sfc_info->sec_32k_en = sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x200000;
            break;
        case 0x8d:
            printk(KERN_NOTICE "RtkSFC MTD: SST 25VF040B detected.\n");
            sfc_info->sec_64k_en = sfc_info->sec_32k_en = sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x80000;
            break;
        default:
            printk(KERN_NOTICE "RtkSFC MTD: SST unknown id2=0x%x detected.\n",
                   sfc_info->device_id2);
            break;
        }
        break;
    default:
        printk(KERN_NOTICE "RtkSFC MTD: SST unknown id1=0x%x detected.\n",
               sfc_info->device_id1);
        break;
    }
    if(sfc_info->erase_opcode==0xFFFFFFFF)//Set to default.
    {
        SFC_4KB_ERASE;
    }
    return 0;
}

/*--------------------------------------------------------------------------------
  SPANSION serial flash information list
  [SPANSION S25FL004A ]
  erase size: 64KB

  [SPANSION S25FL008A ]
  erase size: 64KB

  [SPANSION S25FL016A ]
  erase size: 64KB

  [SPANSION S25FL032A ]
  erase size: 64KB

  [SPANSION S25FL064A ]
  erase size: 64KB

  [SPANSION S25FL128P](256K sector)
  erase size: 64KB / 256KB

  [SPANSION S25FL129P](256K sector)
  erase size: 64KB / 256KB
  --------------------------------------------------------------------------------*/
static int spansion_init(rtk_sfc_info_t *sfc_info) {
    switch(sfc_info->device_id1) {
    case 0x02:
        switch(sfc_info->device_id2) {
        case 0x12:
            printk(KERN_NOTICE "RtkSFC MTD: SPANSION S25FL004A detected.\n");
            sfc_info->sec_64k_en = SUPPORTED;
            sfc_info->sec_256k_en = sfc_info->sec_32k_en = sfc_info->sec_4k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x80000;
            break;
        case 0x13:
            printk(KERN_NOTICE "RtkSFC MTD: SPANSION S25FL008A detected.\n");
            sfc_info->sec_64k_en = SUPPORTED;
            sfc_info->sec_256k_en = sfc_info->sec_32k_en = sfc_info->sec_4k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x100000;
            break;
        case 0x14:
            printk(KERN_NOTICE "RtkSFC MTD: SPANSION S25FL016A detected.\n");
            sfc_info->sec_64k_en = SUPPORTED;
            sfc_info->sec_256k_en = sfc_info->sec_32k_en = sfc_info->sec_4k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x200000;
            break;
        case 0x15:
            printk(KERN_NOTICE "RtkSFC MTD: SPANSION S25FL032A detected.\n");
            sfc_info->sec_64k_en = SUPPORTED;
            sfc_info->sec_256k_en = sfc_info->sec_32k_en = sfc_info->sec_4k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x400000;
            break;
        case 0x16:
            printk(KERN_NOTICE "RtkSFC MTD: SPANSION S25FL064A detected.\n");
            sfc_info->sec_64k_en = SUPPORTED;
            sfc_info->sec_256k_en = sfc_info->sec_32k_en = sfc_info->sec_4k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x800000;
            break;
        default:
            printk(KERN_NOTICE "RtkSFC MTD: SPANSION unknown id2=0x%x detected.\n",
                   sfc_info->device_id2);
            break;
        }
        break;
    default:
        printk(KERN_NOTICE "RtkSFC MTD: SPANSION unknown id1=0x%x detected.\n",
               sfc_info->device_id1);
        break;
    }
    if(sfc_info->erase_opcode==0xFFFFFFFF)//Set to default.
    {
        SFC_64KB_ERASE;
    }

    return 0;
}

/*--------------------------------------------------------------------------------
  MXIC serial flash information list
  [MXIC MX25L4005]
  erase size: 4KB / 64KB

  [MXIC MX25L8005 / MX25L8006E]
  erase size: 4KB / 64KB

  [MXIC MX25L1605]
  erase size: 4KB / 64KB

  [MXIC MX25L3205]
  erase size: 4KB / 64KB

  [MXIC MX25L6405D]
  erase size: 4KB / 64KB


  [MXIC MX25L6445E]
  erase size: 4KB / 32KB / 64KB

  [MXIC MX25L12845E]
  erase size: 4KB / 32KB / 64KB

  [MXIC MX25L12805E]
  erase size: 4KB / 64KB

  [MXIC MX25L25635E]
  erase size: 4KB / 32KB / 64KB

  [MXIC MX25L6455E]
  erase size:  4KB / 32KB / 64KB

  [MXIC MX25L12855E]
  erase size: 4KB / 32KB / 64KB

  --------------------------------------------------------------------------------*/

static int mxic_init(rtk_sfc_info_t *sfc_info) {
    unsigned char manufacturer_id, device_id;

    switch(sfc_info->device_id1) {
    case 0x20:
        switch(sfc_info->device_id2) {
        case 0x13:
            printk(KERN_NOTICE "RtkSFC MTD: MXIC MX25L4005 detected.\n");
            SFC_4KB_ERASE;
            sfc_info->sec_64k_en = sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = sfc_info->sec_32k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x80000;
            break;
        case 0x14:
            printk(KERN_NOTICE "RtkSFC MTD: MXIC MX25L8005/MX25L8006E detected.\n");
            SFC_4KB_ERASE;
            sfc_info->sec_64k_en = sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = sfc_info->sec_32k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x100000;
            break;
        case 0x15:
            printk(KERN_NOTICE "RtkSFC MTD: MXIC MX25L1605 detected.\n");
            SFC_4KB_ERASE;
            sfc_info->sec_64k_en = sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = sfc_info->sec_32k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x200000;
            break;
        case 0x16:
            printk(KERN_NOTICE "RtkSFC MTD: MXIC MX25L3205 detected.\n");
            SFC_4KB_ERASE;
            sfc_info->attr |= RTK_SFC_ATTR_SUPPORT_DUAL_O;
            sfc_info->sec_64k_en = sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = sfc_info->sec_32k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x400000;
            break;
        case 0x17:
            manufacturer_id = (sfc_info->sfc.flash_id & 0x00FF0000)>>16;
            device_id = (sfc_info->sfc.flash_id & 0x0000FF00)>>8;
            if(manufacturer_id == 0xc2 && device_id == 0x16) {
                printk(KERN_NOTICE "RtkSFC MTD: MXIC MX25L6445E detected....\n");
                SFC_4KB_ERASE;
                //sfc_info->attr |= RTK_SFC_ATTR_SUPPORT_DUAL_IO;
                sfc_info->sec_64k_en = sfc_info->sec_32k_en = sfc_info->sec_4k_en = SUPPORTED;
                sfc_info->sec_256k_en = NOT_SUPPORTED;
            }
            else {
                printk(KERN_NOTICE "RtkSFC MTD: MXIC MX25L6405D detected.\n");
                SFC_4KB_ERASE;
                sfc_info->sec_64k_en = sfc_info->sec_4k_en = SUPPORTED;
                sfc_info->sec_256k_en = sfc_info->sec_32k_en = NOT_SUPPORTED;
            }
            sfc_info->mtd_info->size = 0x800000;
            break;
        case 0x18:
            manufacturer_id = (sfc_info->sfc.flash_id & 0x00FF0000)>>16;
            device_id = (sfc_info->sfc.flash_id & 0x0000FF00)>>8;
            if(manufacturer_id == 0xc2 && device_id == 0x17) {
                printk(KERN_NOTICE "RtkSFC MTD: MXIC MX25L12845E detected.\n");
                SFC_4KB_ERASE;
                sfc_info->attr |= RTK_SFC_ATTR_SUPPORT_DUAL_IO|RTK_SFC_ATTR_SUPPORT_DUAL_O;
                sfc_info->sec_64k_en = sfc_info->sec_32k_en = sfc_info->sec_4k_en = SUPPORTED;
                sfc_info->sec_256k_en = NOT_SUPPORTED;
            }
            else {
                printk(KERN_NOTICE "RtkSFC MTD: MXIC MX25L12805 detected.\n");
                SFC_4KB_ERASE;
                sfc_info->sec_64k_en = sfc_info->sec_4k_en = SUPPORTED;
                sfc_info->sec_256k_en = sfc_info->sec_32k_en = NOT_SUPPORTED;
            }
            sfc_info->mtd_info->size = 0x1000000;
            break;
        case 0x19:
            manufacturer_id = (sfc_info->sfc.flash_id & 0x00FF0000)>>16;
            device_id = (sfc_info->sfc.flash_id & 0x0000FF00)>>8;
            if(manufacturer_id == 0xc2 && device_id == 0x18) {
                printk(KERN_NOTICE "RtkSFC MTD: MXIC MX25L25635E detected.\n");
                SFC_4KB_ERASE;
                sfc_info->attr |= RTK_SFC_ATTR_SUPPORT_DUAL_IO;
                sfc_info->sec_64k_en = sfc_info->sec_32k_en = sfc_info->sec_4k_en = SUPPORTED;
                sfc_info->sec_256k_en = NOT_SUPPORTED;
                sfc_info->mtd_info->size = 0x2000000;
            }
            else {
                printk(KERN_NOTICE "RtkSFC MTD: MXIC unknown mnftr_id=0x%x, dev_id=0x%x .\n", manufacturer_id, device_id) ;
                SFC_4KB_ERASE;
                sfc_info->sec_4k_en = SUPPORTED;
                sfc_info->sec_256k_en = sfc_info->sec_64k_en = sfc_info->sec_32k_en = NOT_SUPPORTED;
            }
            break;

        default:
            printk(KERN_NOTICE "RtkSFC MTD: MXIC unknown id2=0x%x detected.\n",
                   sfc_info->device_id2);
            SFC_4KB_ERASE;
            sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = sfc_info->sec_64k_en = sfc_info->sec_32k_en = NOT_SUPPORTED;
            break;
        }
        break;
    case 0x26:////add by alexchang 1206-2010
        switch(sfc_info->device_id2) {
        case 0x17:
            printk(KERN_NOTICE "RtkSFC MTD: MXIC MX25L6455E detected.\n");
            SFC_4KB_ERASE;
            sfc_info->sec_64k_en = sfc_info->sec_32k_en = sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x800000;
            break;

        case 0x18:
            printk(KERN_NOTICE "RtkSFC MTD: MXIC MX25L12855E detected.\n");
            SFC_4KB_ERASE;
            sfc_info->sec_64k_en = sfc_info->sec_32k_en = sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x1000000;
            break;

        default:
            printk(KERN_NOTICE "RtkSFC MTD: MXIC unknown id2=0x%x detected.\n",
                   sfc_info->device_id2);
            break;
        }
        break;
    default:
        printk(KERN_NOTICE "RtkSFC MTD: MXIC unknown id1=0x%x detected.\n",
               sfc_info->device_id1);
        break;
    }

    if(sfc_info->erase_opcode==0xFFFFFFFF)//Set to default.
    {
        SFC_64KB_ERASE;
    }

    return 0;

}
/*--------------------------------------------------------------------------------
  PMC serial flash information list

  --------------------------------------------------------------------------------*/

static int pmc_init(rtk_sfc_info_t *sfc_info) {
    switch(sfc_info->device_id1) {
    case 0x9d:
        switch(sfc_info->device_id2) {
        case 0x7c:
            printk(KERN_NOTICE "RtkSFC MTD: PMC Pm25LV010 detected.\n");
            sfc_info->mtd_info->size = 0x20000;
            break;
        case 0x7d:
            printk(KERN_NOTICE "RtkSFC MTD: PMC Pm25LV020 detected.\n");
            sfc_info->mtd_info->size = 0x40000;
            break;
        case 0x7e:
            printk(KERN_NOTICE "RtkSFC MTD: PMC Pm25LV040 detected.\n");
            sfc_info->mtd_info->size = 0x80000;
            break;
        case 0x46:
            printk(KERN_NOTICE "RtkSFC MTD: PMC Pm25LQ032 detected.\n");
            SFC_4KB_ERASE;
            sfc_info->sec_64k_en = sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = sfc_info->sec_32k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x400000;
            break;
        default:
            printk(KERN_NOTICE "RtkSFC MTD: PMC unknown id2=0x%x detected.\n",
                   sfc_info->device_id2);
            break;
        }
        break;
    default:
        printk(KERN_NOTICE "RtkSFC MTD: PMC unknown id1=0x%x detected.\n",
               sfc_info->device_id1);
        break;
    }
    sfc_info->erase_size    = 0x1000;   //4KB
    sfc_info->erase_opcode  = 0x000000d7;   //for 4KB erase.
    return 0;
}

/*--------------------------------------------------------------------------------
  STM serial flash information list
  [ST M25P128]
  erase size: 2MB

  [ST M25Q128]

  [STM N25Q032]
  erase size: 4KB / 64KB

  [STM N25Q064]
  erase size: 4KB / 64KB
  --------------------------------------------------------------------------------*/

static int stm_init(rtk_sfc_info_t *sfc_info) {
    switch(sfc_info->device_id1) {
    case 0x20:
        switch(sfc_info->device_id2) {
        case 0x17:
            printk(KERN_NOTICE "RtkSFC MTD: ST M25P64 detected.\n");
            SFC_64KB_ERASE;
            sfc_info->sec_64k_en = SUPPORTED;
            sfc_info->sec_256k_en = sfc_info->sec_32k_en = sfc_info->sec_4k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x1000000;
            break;
        case 0x18:
            printk(KERN_NOTICE "RtkSFC MTD: ST M25P128 detected.\n");
            SFC_256KB_ERASE;
            sfc_info->sec_256k_en = SUPPORTED;
            sfc_info->sec_64k_en = sfc_info->sec_32k_en = sfc_info->sec_4k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x1000000;
            break;
        default:
            printk(KERN_NOTICE "RtkSFC MTD: ST unknown id2=0x%x detected.\n",   sfc_info->device_id2);
            SFC_64KB_ERASE;
            sfc_info->sec_64k_en = SUPPORTED;
            sfc_info->sec_256k_en = sfc_info->sec_32k_en = sfc_info->sec_4k_en = NOT_SUPPORTED;
            break;
        }
        break;

    case 0xba:
        switch(sfc_info->device_id2) {
        case 0x16:
            printk(KERN_NOTICE "RtkSFC MTD: ST N25Q032 detected.\n");
            SFC_4KB_ERASE;
            sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = sfc_info->sec_64k_en = sfc_info->sec_32k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x400000;
            break;
        case 0x17:
            printk(KERN_NOTICE "RtkSFC MTD: ST N25Q064 detected.\n");
            SFC_4KB_ERASE;
            sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = sfc_info->sec_64k_en = sfc_info->sec_32k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x800000;
            break;
        case 0x18:
            printk(KERN_NOTICE "RtkSFC MTD: ST N25Q128 detected.\n");
            SFC_64KB_ERASE;
            sfc_info->sec_64k_en = SUPPORTED;
            sfc_info->sec_256k_en = sfc_info->sec_32k_en = sfc_info->sec_4k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x1000000;
            break;
        default:
            printk(KERN_NOTICE "RtkSFC MTD: ST unknown id2=0x%x detected.\n",   sfc_info->device_id2);
            SFC_64KB_ERASE;
            sfc_info->sec_64k_en = SUPPORTED;
            sfc_info->sec_256k_en = sfc_info->sec_32k_en = sfc_info->sec_4k_en = NOT_SUPPORTED;
            break;
        }
        break;
    default:
        printk(KERN_NOTICE "RtkSFC MTD: ST unknown id1=0x%x detected.\n",   sfc_info->device_id1);
        SFC_64KB_ERASE;
        sfc_info->sec_64k_en = SUPPORTED;
        sfc_info->sec_256k_en = sfc_info->sec_32k_en = sfc_info->sec_4k_en = NOT_SUPPORTED;
        break;
    }
    if(sfc_info->erase_opcode==0xFFFFFFFF)//Set to default.
    {
        SFC_64KB_ERASE;
    }

    return 0;
}


/*--------------------------------------------------------------------------------
  EON serial flash information list
  [EON EN25B64-100FIP]64Mbits
  erase size: 64KB

  [EON EN25F16]
  erase size: 4KB / 64KB

  [EON EN25Q64]
  erase size: 4KB


  [EON EN25Q128]
  erase size: 4KB / 64KB
  --------------------------------------------------------------------------------*/
static int eon_init(rtk_sfc_info_t *sfc_info) {
    switch(sfc_info->device_id1) {
    case 0x20:
        switch(sfc_info->device_id2) {
        case 0x17:
            printk(KERN_NOTICE "RtkSFC MTD: EON EN25B64-100FIP detected.\n");
            SFC_64KB_ERASE;
            sfc_info->sec_64k_en = SUPPORTED;
            sfc_info->sec_256k_en = sfc_info->sec_32k_en = sfc_info->sec_4k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x800000;
            break;
        default:
            printk(KERN_NOTICE "RtkSFC MTD: EON unknown id2=0x%x detected.\n",
                   sfc_info->device_id2);
            SFC_4KB_ERASE;
            sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = sfc_info->sec_64k_en = sfc_info->sec_32k_en = NOT_SUPPORTED;
            break;
        }
        return 0;

    case 0x31:
        switch(sfc_info->device_id2) {
        case 0x15:
            printk(KERN_NOTICE "RtkSFC MTD: EON EN25F16 detected.\n");
            SFC_4KB_ERASE;
            sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = sfc_info->sec_64k_en = sfc_info->sec_32k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x200000;
            break;
        default:
            printk(KERN_NOTICE "RtkSFC MTD: EON unknown id2=0x%x detected.\n",
                   sfc_info->device_id2);
            break;
        }
        return 0;

    case 0x30:
        switch(sfc_info->device_id2) {
        case 0x16:
            printk(KERN_NOTICE "RtkSFC MTD: EON EN25Q32 detected.\n");
            SFC_4KB_ERASE;
            sfc_info->sec_64k_en = sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = sfc_info->sec_32k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x400000;
            break;
        case 0x17:
            printk(KERN_NOTICE "RtkSFC MTD: EON EN25Q64 detected.\n");
            SFC_4KB_ERASE;
            sfc_info->sec_64k_en = sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = sfc_info->sec_32k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x800000;
            break;
        case 0x18:
            printk(KERN_NOTICE "RtkSFC MTD: EON EN25Q128 detected.\n");
            SFC_4KB_ERASE;
            sfc_info->sec_64k_en = sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = sfc_info->sec_32k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x1000000;
            break;
        default:
            printk(KERN_NOTICE "RtkSFC MTD: EON unknown id2=0x%x detected.\n",
                   sfc_info->device_id2);
            break;
        }
        return 0;

    case 0x70:
        switch(sfc_info->device_id2) {
        case 0x15:
            printk(KERN_NOTICE "RtkSFC MTD: EON EON_EN25QH16 detected.\n");
            SFC_4KB_ERASE;
            sfc_info->sec_64k_en = sfc_info->sec_32k_en = sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x1000000;
            break;
        case 0x18:
            printk(KERN_NOTICE "RtkSFC MTD: EON EON_EN25QH16128A detected.\n");
            SFC_4KB_ERASE;
            sfc_info->sec_64k_en = sfc_info->sec_32k_en = sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x1000000;
            break;
        default:
            printk(KERN_NOTICE "RtkSFC MTD: EON unknown id2=0x%x detected.\n",
                   sfc_info->device_id2);
            break;
        }
        return 0;
    default:
        printk(KERN_NOTICE "RtkSFC MTD: EON unknown id1=0x%x detected.\n",
               sfc_info->device_id1);
        break;
    }

    // set to default.
    if(sfc_info->erase_opcode == 0xFFFFFFFF) {
        SFC_64KB_ERASE;
    }

    return 0;
}

/*--------------------------------------------------------------------------------
  ATMEL serial flash information list
  [ATMEL AT25DF641A]64Mbits
  erase size: 64KB

  [ATMEL AT25DF321A]
  erase size: 4KB / 64KB
  --------------------------------------------------------------------------------*/
static int atmel_init(rtk_sfc_info_t *sfc_info) {
    switch(sfc_info->device_id1) {
    case 0x48:
    case 0x86:
        switch(sfc_info->device_id2) {
        case 0x1:
        case 0x0:
            printk(KERN_NOTICE "RtkSFC MTD: AT25DF641A detected.\n");
            SFC_4KB_ERASE;
            sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = sfc_info->sec_64k_en = sfc_info->sec_32k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x800000;
            break;
        default:
            printk(KERN_NOTICE "RtkSFC MTD: AT25DF641A unknown id2=0x%x detected.\n",
                   sfc_info->device_id2);
            SFC_4KB_ERASE;
            sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = sfc_info->sec_64k_en = sfc_info->sec_32k_en = NOT_SUPPORTED;
            break;
        }
        return 0;

    case 0x47:
        switch(sfc_info->device_id2) {
        case 0x1:
        case 0x0:
            printk(KERN_NOTICE "RtkSFC MTD: AT25DF321A detected.\n");
            SFC_4KB_ERASE;
            sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = sfc_info->sec_64k_en = sfc_info->sec_32k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x400000;
            break;
        default:
            printk(KERN_NOTICE "RtkSFC MTD: AT25DF321A like unknown id2=0x%x detected.\n",
                   sfc_info->device_id2);
            SFC_4KB_ERASE;
            sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = sfc_info->sec_64k_en = sfc_info->sec_32k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x400000;
            break;
        }
        return 0;

    default:
        printk(KERN_NOTICE "RtkSFC MTD: EON unknown id1=0x%x detected.\n",
               sfc_info->device_id1);
        break;
    }

    // set to default.
    if(sfc_info->erase_opcode == 0xFFFFFFFF) {
        SFC_4KB_ERASE;
    }

    return 0;
}

/*--------------------------------------------------------------------------------
  WINBOND serial flash information list
  [WINBOND 25Q128BVFG]
  erase size:

  [WINBOND W25Q32BV]32 Mbits
  erase size:4KB /32KB /64KB

  [SPANSION S25FL064K ] 64Mbits //SPANSION brand, Winbond OEM
  erase size: 4KB / 32KB / 64KB

  --------------------------------------------------------------------------------*/
static int winbond_init(rtk_sfc_info_t *sfc_info) {
    switch(sfc_info->device_id1) {
    case 0x40:
        switch(sfc_info->device_id2) {
        case 0x14:
            printk(KERN_NOTICE "RtkSFC MTD: WINBOND W25Q80BV detected.\n");
            SFC_4KB_ERASE;
            sfc_info->attr |= RTK_SFC_ATTR_SUPPORT_DUAL_IO|RTK_SFC_ATTR_SUPPORT_DUAL_O;
            sfc_info->sec_64k_en = sfc_info->sec_32k_en = sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x100000;
            break;
        case 0x19:
            printk(KERN_NOTICE "RtkSFC MTD: WINBOND S25FL256K detected.\n");
            SFC_4KB_ERASE;
            sfc_info->sec_4k_en = sfc_info->sec_32k_en = sfc_info->sec_64k_en = SUPPORTED;
            sfc_info->sec_256k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x2000000;
            //louis
#if 0
            FLASH_BASE = (unsigned char*)0xbdc00000;
            FLASH_POLL_ADDR = (unsigned char*)0xbec00000;
#endif
            break;
        case 0x16:
            printk(KERN_NOTICE "RtkSFC MTD: WINBOND W25Q32BV(W25Q32FV) detected.\n");
            SFC_4KB_ERASE;
            sfc_info->attr |= RTK_SFC_ATTR_SUPPORT_DUAL_IO|RTK_SFC_ATTR_SUPPORT_DUAL_O;
            sfc_info->sec_64k_en = sfc_info->sec_32k_en = sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x400000;
            break;
        case 0x17:
            printk(KERN_NOTICE "RtkSFC MTD: SPANSION S25FL064K detected.\n");
            SFC_4KB_ERASE;
            sfc_info->attr |= RTK_SFC_ATTR_SUPPORT_DUAL_IO|RTK_SFC_ATTR_SUPPORT_DUAL_O;
            sfc_info->sec_4k_en = sfc_info->sec_32k_en = sfc_info->sec_64k_en = SUPPORTED;
            sfc_info->sec_256k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x800000;
            break;
        case 0x18:
            printk(KERN_NOTICE "RtkSFC MTD: WINBOND 25Q128BVFG(W25Q128BVFIG) detected.\n");
            SFC_4KB_ERASE;
            sfc_info->attr |= RTK_SFC_ATTR_SUPPORT_DUAL_IO|RTK_SFC_ATTR_SUPPORT_DUAL_O;
            sfc_info->sec_64k_en = sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = sfc_info->sec_32k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x1000000;
            break;
        default:
            printk(KERN_NOTICE "RtkSFC MTD: WINBOND unknown id2=0x%x detected.\n",
                   sfc_info->device_id2);
            SFC_4KB_ERASE;
            sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = sfc_info->sec_64k_en = sfc_info->sec_32k_en = NOT_SUPPORTED;

            break;
        }
        return 0;
    case 0x60:
        switch(sfc_info->device_id2) {
        case 0x16:
            printk(KERN_NOTICE "RtkSFC MTD: WINBOND W25Q32FV(Quad Mode) detected.\n");
            SFC_4KB_ERASE;
            sfc_info->sec_64k_en = sfc_info->sec_32k_en = sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x400000;
            break;
        default:
            printk(KERN_NOTICE "RtkSFC MTD: WINBOND unknown id2=0x%x detected.\n",
                   sfc_info->device_id2);
            SFC_4KB_ERASE;
            sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = sfc_info->sec_64k_en = sfc_info->sec_32k_en = NOT_SUPPORTED;

            break;
        }
        return 0;
    default:
        printk(KERN_NOTICE "RtkSFC MTD: WINBOND unknown id1=0x%x detected.\n",
               sfc_info->device_id1);
        break;
    }
    SFC_4KB_ERASE;
    return 0;
}

/*--------------------------------------------------------------------------------
  ESMT serial flash information list
  [ESMT F25L32PA]32 Mbits
  erase size:4KB / 64KB
  [ESMT F25L64QA]64 Mbits
  erase size:4KB / 32KB / 64KB
  --------------------------------------------------------------------------------*/
static int esmt_init(rtk_sfc_info_t *sfc_info) {
    switch(sfc_info->device_id1) {
    case 0x20:
        switch(sfc_info->device_id2) {
        case 0x16:
            printk(KERN_NOTICE "RtkSFC MTD: ESMT F25L32PA detected.\n");
            SFC_4KB_ERASE;
            sfc_info->sec_64k_en = sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = sfc_info->sec_32k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x400000;
            break;

        default:
            printk(KERN_NOTICE "RtkSFC MTD: ESMT unknown id2=0x%x detected.\n",
                   sfc_info->device_id2);
            SFC_4KB_ERASE;
            sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = sfc_info->sec_64k_en = sfc_info->sec_32k_en = NOT_SUPPORTED;

            break;
        }
        break;
    case 0x40:
        switch(sfc_info->device_id2) {
        case 0x16:
            printk(KERN_NOTICE "RtkSFC MTD: ESMT F25L32PA detected.\n");
            SFC_4KB_ERASE;
            sfc_info->sec_64k_en = sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = sfc_info->sec_32k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x400000;
            break;

        default:
            printk(KERN_NOTICE "RtkSFC MTD: ESMT unknown id2=0x%x detected.\n",
                   sfc_info->device_id2);
            SFC_4KB_ERASE;
            sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = sfc_info->sec_64k_en = sfc_info->sec_32k_en = NOT_SUPPORTED;

            break;
        }
        break;
    case 0x41:
        switch(sfc_info->device_id2) {
        case 0x16:
            printk(KERN_NOTICE "RtkSFC MTD: ESMT F25L32QA detected.\n");
            SFC_4KB_ERASE;
            sfc_info->sec_64k_en = sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = sfc_info->sec_32k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x400000;
            break;

        case 0x17:
            printk(KERN_NOTICE "RtkSFC MTD: ESMT F25L64QA detected.\n");
            SFC_4KB_ERASE;
            sfc_info->sec_64k_en = sfc_info->sec_32k_en = sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = NOT_SUPPORTED;
            sfc_info->mtd_info->size = 0x800000;
            break;

        default:
            printk(KERN_NOTICE "RtkSFC MTD: ESMT unknown id2=0x%x detected.\n",
                   sfc_info->device_id2);
            SFC_4KB_ERASE;
            sfc_info->sec_4k_en = SUPPORTED;
            sfc_info->sec_256k_en = sfc_info->sec_64k_en = sfc_info->sec_32k_en = NOT_SUPPORTED;

            break;
        }
        break;
    default:
        printk(KERN_NOTICE "RtkSFC MTD: ESMT unknown id1=0x%x detected.\n",
               sfc_info->device_id1);
        break;
    }

    if(sfc_info->erase_opcode == 0xFFFFFFFF)//Set to default.
    {
        SFC_64KB_ERASE;
    }

    return 0;
}

int rtk_identify_sfc(rtk_sfc_info_t *sfc_info)
{
    int ret = 0;
    switch(sfc_info->manufacturer_id) {
    case MANUFACTURER_ID_SST:
        ret = sst_init(sfc_info);
        break;

    case MANUFACTURER_ID_SPANSION:
        ret = spansion_init(sfc_info);
        break;
    case MANUFACTURER_ID_MXIC:
        ret = mxic_init(sfc_info);
        break;
    case MANUFACTURER_ID_PMC:
        ret = pmc_init(sfc_info);
        break;
    case MANUFACTURER_ID_STM:
        ret = stm_init(sfc_info);
        break;
    case MANUFACTURER_ID_EON:
        ret = eon_init(sfc_info);
        break;
    case MANUFACTURER_ID_ATMEL:
        ret = atmel_init(sfc_info);
        break;

    case MANUFACTURER_ID_WINBOND:
        ret = winbond_init(sfc_info);
        break;
    case MANUFACTURER_ID_ESMT:
        ret = esmt_init(sfc_info);
        break;
    case MANUFACTURER_ID_GD:
        ret = gd_init(sfc_info);
        break;
    default:
        printk(KERN_ERR "RtkSFC MTD: Unknown flash type.\n");
        printk(KERN_ERR "Manufacturer's ID = %02X, Memory Type = %02X, Memory Capacity = %02X\n", \
               sfc_info->manufacturer_id & 0xff, (sfc_info->manufacturer_id >> 8) & 0xff, (sfc_info->manufacturer_id >> 16) & 0xff);

        return -ENODEV;
        break;
    }
    return ret;
}

#endif
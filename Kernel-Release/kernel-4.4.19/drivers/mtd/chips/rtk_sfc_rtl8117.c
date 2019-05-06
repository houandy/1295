#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/mtd/physmap.h>
#include <linux/module.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/cfi.h>
#include <linux/mtd/partitions.h>
#include <linux/sysctl.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/pm.h>
#include <asm/io.h>
#include <linux/bitops.h>
#include <mtd/mtd-abi.h>
#include <linux/jiffies.h>
#include <linux/delay.h>
#include <linux/time.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include "../mtdcore.h"
#include "rtk_sfc_rtl8117.h"

static rtk_sfc_info_t *rtk_sfc_info[2];
static bool spi1_on = 0;

static int rtl8117_spi_open(struct inode *inode, struct file *file)
{
    return single_open(file, NULL, PDE_DATA(inode));
}

static ssize_t rtl8117_spi_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    char str[128];
    ssize_t len = 0;
    u8 val = spi1_on;

    len = sprintf(str, "%x\n", val);
    copy_to_user(buf, str, len);
    if (*ppos == 0)
        *ppos += len;
    else
        len = 0;

    return len;
}

static int rtk_sfc_dev_init(void);
static int rtk_spi1_remove(void);
static ssize_t rtl8117_spi_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    char tmp[32];
    u8 num;
    u32 val;

    if (buf && !copy_from_user(tmp, buf, sizeof(tmp)))
    {
        num = sscanf(tmp, "%x", &val);
        if ((val == 1) || (val == 0)) {
        } else
            printk(KERN_ERR, "write procfs not support value  = %x \n", val);
    }
    if (val == 1)
        rtk_sfc_dev_init();
    else if (val == 0)
        rtk_spi1_remove();
    return count;
}

static const struct file_operations spi_fops =
{
    .owner = THIS_MODULE,
    .open = rtl8117_spi_open,
    .read = rtl8117_spi_read,
    .write = rtl8117_spi_write,
    .release    = single_release,
};

static int rtk_sfc_read(struct mtd_info *mtd, loff_t from, size_t len,
                        size_t *retlen, u_char *buf)
{
    rtk_sfc_info_t *sfc_info;

    if((sfc_info = (rtk_sfc_info_t*)mtd->priv) == NULL)
        return -EINVAL;
    if(!buf)
        return -EINVAL;

    *retlen = rtk_memcpy((void *)buf, (void *)(sfc_info->flash_base + from), len);

    return 0;
}

static int rtk_sfc_write(struct mtd_info *mtd, loff_t to, size_t len,
                         size_t *retlen, const u_char *buf)
{
    rtk_sfc_info_t *sfc_info;

    if((sfc_info = (rtk_sfc_info_t*)mtd->priv) == NULL)
        return -EINVAL;
    if(!buf)
        return -EINVAL;

    *retlen = rtk_flash_write(&(sfc_info->sfc), (void *)(sfc_info->flash_base + to), (void *)buf, len);
    return 0;
}

static int rtk_sfc_erase(struct mtd_info *mtd, struct erase_info *instr)
{
    rtk_sfc_info_t *sfc_info;
    unsigned int size;
    volatile unsigned char *addr;
    unsigned int erase_addr;
    unsigned int erase_opcode;
    unsigned int erase_size;

    if((sfc_info = (rtk_sfc_info_t*)(mtd->priv)) == NULL)
        return -EINVAL;
    if(instr->addr + instr->len > mtd->size)
        return -EINVAL;

    addr = sfc_info->flash_base + (instr->addr);
    size = instr->len;
    erase_addr = (unsigned int)instr->addr;
    erase_opcode = sfc_info->erase_opcode;
    erase_size = mtd->erasesize;

    for(size = instr->len ; size > 0 ; size -= erase_size)
    {
        /* choose erase sector size */
        if (((erase_addr&(0x10000-1)) == 0) && (size >= 0x10000) && (sfc_info->sec_64k_en == SUPPORTED))
        {
            erase_opcode = FLASH_BE_COM;
            erase_size = 0x10000;
        }
        else if (((erase_addr&(0x1000-1)) == 0) && (size >= 0x1000) && (sfc_info->sec_4k_en == SUPPORTED))
        {
            erase_opcode = FLASH_CHIP_SEC;
            erase_size = 0x1000;
        }
        else
        {
            erase_opcode = sfc_info->erase_opcode;
            erase_size = mtd->erasesize;
        }
        Flash_erase(&(sfc_info->sfc), (u32)addr & 0x00FFFFFF, erase_opcode);
        addr += erase_size;
        erase_addr += erase_size;
    }

    instr->state = MTD_ERASE_DONE;
    mtd_erase_callback(instr);

    return 0;
}

#if defined(CONFIG_MTD_UBI) || defined(CONFIG_JFFS2_FS)
//fixbug:UBI can accept leb size > 15Kbytes.
static void rtk_sfc_adjust_erase_size(struct mtd_info *mtd)
{
    rtk_sfc_info_t *sfc_info;
    if((sfc_info = (rtk_sfc_info_t*)(mtd->priv)) == NULL)
    {
        printk(KERN_ERR "mtd->priv is NULL.\n");
        return;
    }
    if (sfc_info->erase_size == 0x1000)
    {
        if (sfc_info->sec_64k_en == SUPPORTED)
        {
            SFC_64KB_ERASE;
        }
        else if (sfc_info->sec_256k_en == SUPPORTED)
        {
            SFC_256KB_ERASE;
        }
    }
}
#endif
static const char * const part_probes[] = { "cmdlinepart", "ofpart", NULL };
static int rtk_sfc_mtd_attach(struct mtd_info *mtd_info)
{
    int nr_parts = 0;
    struct mtd_partition *parts;

    /*
     * Partition selection stuff.
     */
#ifdef CONFIG_MTD_CMDLINE_PARTS
    nr_parts = parse_mtd_partitions(mtd_info, part_probes, &parts, 0);
#endif

    if(nr_parts <= 0) {
        if(add_mtd_device(mtd_info)) {
            printk(KERN_WARNING "Rtk SFC: (for SST/SPANSION/MXIC/WINBOND SPI-Flash) Failed to register new device\n");
            return -EAGAIN;
        }
    }
    else
        add_mtd_partitions(mtd_info, parts, nr_parts);

    printk(KERN_INFO "Rtk SFC: (for SST/SPANSION/MXIC SPI Flash)\n");
    return 0;
}

static int rtk_sfc_mtd_detach(struct mtd_info *mtd_info)
{
    int nr_parts = 0;
    struct mtd_partition *parts;

    /*
     * Partition selection stuff.
     */
#ifdef CONFIG_MTD_CMDLINE_PARTS
    nr_parts = parse_mtd_partitions(mtd_info, part_probes, &parts, 0);
#endif

    if(nr_parts <= 0) {
        if(del_mtd_device(mtd_info)) {
            printk(KERN_WARNING "Rtk SFC: (for SST/SPANSION/MXIC/WINBOND SPI-Flash) Failed to unregister new device\n");
            return -EAGAIN;
        }
    }
    else
        del_mtd_partitions(mtd_info);

    printk(KERN_INFO "Detach Rtk SFC: (for SST/SPANSION/MXIC SPI Flash)\n");
    return 0;
}

static int rtk_sfc_probe(struct platform_device *pdev)
{
    struct mtd_info *mtd_info;
    rtk_sfc_info_t *sfc_info;
    int ret = 0;

    mtd_info = (struct mtd_info*)(pdev->dev.platform_data);
    if((sfc_info = (rtk_sfc_info_t*)mtd_info->priv) == NULL)
        return -ENODEV;
    //get RDID
    Flash_RDID(&(sfc_info->sfc));
    sfc_info->manufacturer_id = (sfc_info->sfc.flash_id & 0x00FF0000)>>16;
    sfc_info->device_id2 = RDID_DEVICE_EID_1(sfc_info->sfc.flash_id);
    sfc_info->device_id1 = RDID_DEVICE_EID_2(sfc_info->sfc.flash_id);
    printk(KERN_INFO "--RDID Seq: 0x%x | 0x%x | 0x%x\n",sfc_info->manufacturer_id,sfc_info->device_id1,sfc_info->device_id2);
    ret = rtk_identify_sfc(sfc_info);
    if (ret == -ENODEV)
        return ret;

#if defined(CONFIG_MTD_UBI) || defined(CONFIG_JFFS2_FS)
    rtk_sfc_adjust_erase_size(mtd_info);
#endif

    mtd_info->erasesize = sfc_info->erase_size;
    mtd_info->writesize = 0x100;
    mtd_info->writebufsize = 0x100;

    printk(KERN_INFO "Supported Erase Size:%s%s%s%s.\n"
           , sfc_info->sec_256k_en ? " 256KB" : ""
           , sfc_info->sec_64k_en ? " 64KB" : ""
           , sfc_info->sec_32k_en ?" 32KB" : ""
           , sfc_info->sec_4k_en ? " 4KB" : ""
          );

    if((ret = rtk_sfc_mtd_attach((struct mtd_info*)(pdev->dev.platform_data))) != 0) {
        printk(KERN_ERR "[%s]Realtek SFC attach fail\n",__FUNCTION__);
        return ret;
    }
    return 0;
}

static void rtk_sfc_remove(struct platform_device *pdev)
{
    int ret = 0;

    if((ret = rtk_sfc_mtd_detach((struct mtd_info*)(pdev->dev.platform_data))) != 0)
        printk(KERN_ERR "[%s]Realtek SFC detach fail\n",__FUNCTION__);
}


static const struct of_device_id rtk_sfc_of_match[] = {
    {.compatible = "spi-flash",},
    { /* Sentinel */ },
};

static struct platform_driver rtkSFC_driver = {
    .probe      = rtk_sfc_probe,
    .remove      = rtk_sfc_remove,
    .driver     =
    {
        .name   = "RtkSFC",
        .owner  = THIS_MODULE,
        .of_match_table = rtk_sfc_of_match,
    },
};

static int rtk_spi1_remove(void)
{
    struct platform_device *rtkSFC_device = NULL;
    if (!rtk_sfc_info[1])
        return -ENODEV;
    rtkSFC_device = rtk_sfc_info[1]->dev;

    if (rtkSFC_device)
    {
        platform_device_del(rtkSFC_device);
    }

    kfree(rtk_sfc_info[1]);
    rtk_sfc_info[1] = NULL;
    spi1_on = 0;
    return 0;
}

static int rtk_sfc_init_RtkSFC(struct mtd_info *mtd_info, unsigned int ind)
{
    rtk_sfc_info_t *sfc_info;
    int rc = 0;
    struct platform_device *rtkSFC_device = NULL;
    static int reg_done = 0;
    struct device_node *cnp;

    if((sfc_info = (rtk_sfc_info_t*)mtd_info->priv) == NULL)
        return -ENODEV;

    if (!reg_done)
    {
        rc = platform_driver_register(&rtkSFC_driver);
        reg_done = 1;
    }

    if (!rc) {
        rtkSFC_device = sfc_info->dev;
        if (!rtkSFC_device)
        {
            rtkSFC_device = platform_device_alloc(mtd_info->name, 0);
            rtkSFC_device->dev.of_node = of_node_get(sfc_info->np);
            rtkSFC_device->resource = sfc_info->res;
            rtkSFC_device->num_resources = ARRAY_SIZE(sfc_info->res);
            rtkSFC_device->dev.platform_data = mtd_info;
            sfc_info->dev = rtkSFC_device;
        }

        if (rtkSFC_device) {
            rc = platform_device_add(rtkSFC_device);
            if ((sfc_info->manufacturer_id == 0xff)&&
                    (sfc_info->device_id2 == 0xff) &&
                    (sfc_info->device_id1 == 0xff))
                rc = -EPERM;
        }
        else
            rc = -ENOMEM;
        if (rc < 0) {
            platform_device_put(rtkSFC_device);
        }
    }
    if (rc < 0) {
        printk(KERN_ERR "Realtek SFC Driver installation fails.\n\n");
        platform_driver_unregister(&rtkSFC_driver);
        reg_done = 0;
        return -ENODEV;
    } else
    {
        if (ind == 1)
            spi1_on = 1;
        printk(KERN_INFO "Realtek SFC Driver is successfully installing.\n\n");
    }
    return rc;
}
static int rtk_sfc_dev_init(void)
{
    struct device_node *np[2];
    struct device_node *cnp;
    const u32 *prop;
    int size;
    int sfc_count = 0;
    int i = 0;
    int rc = 0;
    static struct mtd_info *descriptor[2];
    static int done = 0;
    static struct proc_dir_entry * spi1_proc_dir = NULL;
    static struct proc_dir_entry * spi1_proc_entry = NULL;
    static char * spi1_dir_name = "spi1";
    static char * spi1_entry_name = "install";

    printk(KERN_INFO "RtkSFC MTD init ...\n");

    sfc_count = rtk_of_find_compatible_node(NULL, NULL, "realtek,rtk-spi", np);
    for (i=0; i<sfc_count; i++)
    {
        if ((done == 0) && (sfc_count-1 == i))
        {
            done = 1;
            continue;
        }

        if (!rtk_sfc_info[i])
        {
            if (!descriptor[i])
                descriptor[i] = kzalloc(sizeof(struct mtd_info), GFP_KERNEL);
            rtk_sfc_info[i] = kzalloc(sizeof(rtk_sfc_info_t), GFP_KERNEL);
            rtk_sfc_info[i]->mtd_info = descriptor[i];

            prop = of_get_property(np[i], "reg", &size);
            if (prop)
            {
                rtk_sfc_info[i]->sfc.base = ioremap(of_read_number(prop, 1), of_read_number(prop+1, 1));
                printk(KERN_INFO "[%s] get spi controller base addr : 0x%x \n",__func__, (unsigned int)(rtk_sfc_info[i]->sfc.base));
            } else
            {
                printk(KERN_ERR "[%s] get spi controller base addr error !!\n",__func__);
            }

            cnp = of_find_node_by_name(np[i], "spi-flash");
            prop = of_get_property(cnp, "memory-map", &size);
            rtk_sfc_info[i]->flash_base = of_read_number(prop, 1);
            rtk_sfc_info[i]->flash_size = of_read_number(prop+1, 1);
            rtk_sfc_info[i]->res[0].start = rtk_sfc_info[i]->flash_base;
            rtk_sfc_info[i]->res[0].end = rtk_sfc_info[i]->res[0].start + rtk_sfc_info[i]->flash_size;
            rtk_sfc_info[i]->res[0].flags = IORESOURCE_MEM;
            rtk_sfc_info[i]->np = cnp;
            rtk_sfc_info[i]->dev = NULL;

            descriptor[i]->priv = rtk_sfc_info[i];
            if (i==0)
                descriptor[i]->name = "RtkSFC0";
            else if (i==1)
                descriptor[i]->name = "RtkSFC1";
            descriptor[i]->size = rtk_sfc_info[i]->flash_size;
            descriptor[i]->flags = MTD_WRITEABLE;
            descriptor[i]->_erase = rtk_sfc_erase;
            descriptor[i]->_read = rtk_sfc_read;
            descriptor[i]->_write = rtk_sfc_write;
            descriptor[i]->owner = THIS_MODULE;
            descriptor[i]->type = MTD_DATAFLASH;//MTD_DATAFLASH for general serial flash
            descriptor[i]->numeraseregions = 0;
            descriptor[i]->oobsize = 0;
            rc = rtk_sfc_init_RtkSFC(descriptor[i], i);
        }
    }

    if (!spi1_proc_dir)
    {
        spi1_proc_dir = proc_mkdir(spi1_dir_name, NULL);
        if (!spi1_proc_dir)
        {
            printk(KERN_ERR,"Create directory \"%s\" failed.\n", spi1_dir_name);
            return -EPERM;
        }
    }
    if (!spi1_proc_entry)
    {
        spi1_proc_entry = proc_create_data(spi1_entry_name, 0666, spi1_proc_dir, &spi_fops, NULL);
        if (!spi1_proc_entry)
        {
            printk(KERN_ERR, "Create file \"%s\"\" failed.\n", spi1_entry_name);
            return -EPERM;
        }
    }

    return rc;
}

static int __init rtk_sfc_init(void)
{
    int i =0;
    for (i=0; i<2; i++)
    {
        if (rtk_sfc_info[i])
            rtk_sfc_info[i] = NULL;
    }

    return rtk_sfc_dev_init();
}


static void __exit rtk_sfc_exit(void)
{
    struct device_node *np[2];
    int sfc_count = 0;
    int i = 0;
    struct mtd_info *descriptor[2];
    sfc_count = rtk_of_find_compatible_node(NULL, NULL, "realtek,rtk-spi", np);
    if (sfc_count)
        platform_driver_unregister(&rtkSFC_driver);
    for (i=0; i<sfc_count; i++)
    {
        descriptor[i] = rtk_sfc_info[i]->mtd_info;
        del_mtd_device(descriptor[i]);
        kfree(descriptor[i]);
        kfree(rtk_sfc_info[i]);
    }
}

module_init(rtk_sfc_init);
module_exit(rtk_sfc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("vera_xu<vera_xu@realsil.com.cn>");
MODULE_DESCRIPTION("MTD chip driver for Realtek Rtk Serial Flash Controller");

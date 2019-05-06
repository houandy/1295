#include <linux/types.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/dma-mapping.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/platform_device.h>
#include <rtl8117_platform.h>
#include <linux/of_gpio.h>

#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/ioctl.h>

#include "rtl8117-cmac.h"



#define MODULENAME "rtl8117-cmac"

/* write/read MMIO register */
#define RTL_W8(reg, val8)	writeb((val8), ioaddr + (reg))
#define RTL_W16(reg, val16) writew((val16), ioaddr + (reg))
#define RTL_W32(reg, val32) writel((val32), ioaddr + (reg))
#define RTL_R8(reg)		readb(ioaddr + (reg))
#define RTL_R16(reg)		readw(ioaddr + (reg))
#define RTL_R32(reg)		readl(ioaddr + (reg))

#define PA2VA(vaddr)            ((u32) (vaddr) | 0x80000000)
#define VA2PA(vaddr)            ((u32) (vaddr) & ~0x80000000)

struct rtl8117_cmac_controller {
        struct platform_device *pdev;
        int irq;
        void __iomem  *cmac_addr;
	void __iomem  *oobmac_addr;
	void __iomem  *kvm_addr;
	struct TxDesc *CMAC_TxDescArray;	/* 256-aligned Tx descriptor ring */
	struct RxDesc *CMAC_RxDescArray;	/* 256-aligned Rx descriptor ring */
	dma_addr_t CMAC_TxPhyAddr;
	dma_addr_t CMAC_RxPhyAddr;
	void *Tx_databuff[CMAC_NUM_TX_DESC];
	void *Rx_databuff[CMAC_NUM_RX_DESC];	/* Rx data buffers */
	u8 hwtxptr_cmac;
	u8 hwrxptr_cmac;
	bool CMAC_TX_Need_Disable;
	bool CMAC_RX_Need_Disable;
	u8 DASH_ib_write_buffer[100];
};

enum rtl_desc_bit {
	/* First doubleword. */
	DescOwn		= (1 << 31), /* Descriptor is owned by NIC */
	RingEnd		= (1 << 30), /* End of descriptor ring */
	FirstFrag	= (1 << 29), /* First segment of a packet */
	LastFrag	= (1 << 28), /* Final segment of a packet */
};
#define RsvdMask	0x3fffc000

/* CMAC controller */
#define CMAC_IOBASE			0xBAF20000
#define CMAC_OCR0			0x0000
#define CMAC_OCR1			0x0001
#define CMAC_OCR2			0x0002
#define CMAC_OCR3			0x0003
#define CMAC_ISR			0x000E
#define CMAC_IMR			0x000C
#define CMAC_IMR1			0x000D
#define CMAC_RDSAR			0x0004
#define CMAC_TNPDS			0x0008

#define CMACRxdescNumber		4 //Max.=16
#define CMACTxdescNumber		4 //Max.=16

#define CMAC_simulation_case1	1//CMAC RX check incremental data
#define CMAC_simulation_case2	0//CMAC TX send incremental data
#define CMAC_simulation_case3	0//CMAC TX send back data from CMAC RX

#define handshaking_chk		0

//#define rx_buf_sz 1523
static int rx_buf_sz = 1523;
//unsigned char* DASH_ib_write_buffer;
u8 *DASH_ib_write_buffer;
unsigned long DASH_ib_write_length;
unsigned char* DASH_OS_PSH_Buf;


struct proc_dir_entry * proc_cmac_dir = NULL;
struct proc_dir_entry * proc_cmac_entry = NULL;

static char * dir_name = "rtl8117-cmac";
static char * entry_name = "cmac_enabled";

//static bool cmac_enabled = 0;


static inline void cmac_make_unusable_by_driver(struct TxDesc *desc)
{
        desc->addr = cpu_to_le64(0x0badbadbadbadbadull);
        desc->opts1 &= ~cpu_to_le32(RsvdMask);
}

static inline void cmac_make_unusable_by_asic(struct RxDesc *desc)
{
	desc->addr = cpu_to_le64(0x0badbadbadbadbadull);
	desc->opts1 &= ~cpu_to_le32(DescOwn | RsvdMask);
}

static inline void cmac_mark_as_last_descriptor(struct RxDesc *desc)
{
	desc->opts1 |= cpu_to_le32(RingEnd);
}

static inline void cmac_mark_to_driver(struct TxDesc *desc, u32 rx_buf_sz)
{
        u32 eor = le32_to_cpu(desc->opts1) & RingEnd;

        desc->opts1 = cpu_to_le32(eor | rx_buf_sz);
}

static inline void cmac_mark_to_asic(struct RxDesc *desc, u32 rx_buf_sz)
{
	u32 eor = le32_to_cpu(desc->opts1) & RingEnd;

	desc->opts1 = cpu_to_le32(DescOwn | eor | rx_buf_sz);
}

static inline void cmac_tx_map_to_driver(struct TxDesc *desc, dma_addr_t mapping,
                                           u32 rx_buf_sz)
{
        desc->addr = cpu_to_le64(mapping);
        wmb();
	cmac_mark_to_driver(desc, rx_buf_sz);
}

static inline void cmac_map_to_asic(struct RxDesc *desc, dma_addr_t mapping,
					   u32 rx_buf_sz)
{
	desc->addr = cpu_to_le64(mapping);
	wmb();
	cmac_mark_to_asic(desc, rx_buf_sz);
}

static inline void *cmac_align(void *data)
{
	return (void *)ALIGN((long)data, 16);
}

static struct sk_buff *cmac_alloc_tx_data(struct rtl8117_cmac_controller *tp,
                                                 struct TxDesc *desc)
{
        void *data;
        dma_addr_t mapping;
        struct device *d = &tp->pdev->dev;
        //struct net_device *dev = tp->dev;
        //int node = dev->dev.parent ? dev_to_node(dev->dev.parent) : -1;
        int node = -1;

        data = kmalloc_node(rx_buf_sz, GFP_KERNEL, node);
        if (!data)
                return NULL;

        if (cmac_align(data) != data) {
                kfree(data);
                data = kmalloc_node(rx_buf_sz + 15, GFP_KERNEL, node);
                if (!data)
                        return NULL;
        }

        mapping = dma_map_single(d, cmac_align(data), rx_buf_sz,
                                 DMA_FROM_DEVICE);
        if (unlikely(dma_mapping_error(d, mapping))) {
                dev_err(d, "Failed to map RX DMA!\n");
                goto err_out;
        }

        cmac_tx_map_to_driver(desc, mapping, rx_buf_sz);
        return data;

err_out:
        kfree(data);
        return NULL;
}

static struct sk_buff *cmac_alloc_rx_data(struct rtl8117_cmac_controller *tp,
						 struct RxDesc *desc)
{
	void *data;
	dma_addr_t mapping;
	struct device *d = &tp->pdev->dev;
	//struct net_device *dev = tp->dev;
	//int node = dev->dev.parent ? dev_to_node(dev->dev.parent) : -1;
	int node = -1;

	data = kmalloc_node(rx_buf_sz, GFP_KERNEL, node);
	if (!data)
		return NULL;

	if (cmac_align(data) != data) {
		kfree(data);
		data = kmalloc_node(rx_buf_sz + 15, GFP_KERNEL, node);
		if (!data)
			return NULL;
	}

	mapping = dma_map_single(d, cmac_align(data), rx_buf_sz,
				 DMA_FROM_DEVICE);
	if (unlikely(dma_mapping_error(d, mapping))) {
		dev_err(d, "Failed to map RX DMA!\n");
		goto err_out;
	}

	cmac_map_to_asic(desc, mapping, rx_buf_sz);
	return data;

err_out:
	kfree(data);
	return NULL;
}


void bsp_disable_cmac_rx(struct rtl8117_cmac_controller *tp)
{
	void __iomem *ioaddr = tp->cmac_addr;
 //   u8 i;
    //INT16U tmp;
    u8 tmp1;
printk(KERN_ERR "[CMAC] %s: Rx \n", __func__);

/*
    for(i=0; i<CMACRxdescNumber; i++)
    {
        free((void*)(PA2VA(REG32(CMACRxdescStartAddr+i*16+8))));
    }
*/
    //tmp=REG16(CMAC_IOBASE+CMAC_IMR);
    // REG16(CMAC_IOBASE+CMAC_IMR)=0xFFFC&tmp;
    //Disable RX
    tmp1= RTL_R8(CMAC_OCR0);
    tmp1&=0xFE;
    RTL_W8(CMAC_OCR0,tmp1);
    //	hwrxptr_cmac=0;
    
}

void bsp_enable_cmac_rx(struct rtl8117_cmac_controller *tp)
{
	void __iomem *ioaddr = tp->cmac_addr;
    u16 tmp=0;

 /*   //Rx descriptor setup
    for(i=0; i<CMACRxdescNumber; i++)
    {
        if(i == (CMACRxdescNumber - 1))
            REG32(CMACRxdescStartAddr+i*16)=0xC0000000;
        else
            REG32(CMACRxdescStartAddr+i*16)=0x80000000;

        REG32(CMACRxdescStartAddr+i*16+4)=0x0;
        REG32(CMACRxdescStartAddr+i*16+8)=VA2PA(malloc(0x600));
        REG32(CMACRxdescStartAddr+i*16+12)=0x0;
    }
*/
	unsigned int i;
	void *data;

printk(KERN_ERR "[CMAC] %s: Rx \n", __func__);
	for(i=0; i<CMAC_NUM_RX_DESC; i++){
		//void *data;
		if(tp->Rx_databuff[i]){
			cmac_mark_to_asic(tp->CMAC_RxDescArray + i, rx_buf_sz);
			//printk(KERN_ERR "[CMAC] %s: reset rx \n", __func__);
		}else{
			data = cmac_alloc_rx_data(tp, tp->CMAC_RxDescArray + i);
			if (!data) {
				cmac_make_unusable_by_asic(tp->CMAC_RxDescArray + i);
				dev_err(&tp->pdev->dev, "can't allocate rx buffer memory\n");
			}
			//printk(KERN_ERR "[CMAC] %s: alloc rx \n", __func__);
			tp->Rx_databuff[i] = data;
		}
	}


	cmac_mark_as_last_descriptor(tp->CMAC_RxDescArray + CMAC_NUM_RX_DESC - 1);
	

	RTL_W32(CMAC_RDSAR, ((u64) tp->CMAC_RxPhyAddr) & DMA_BIT_MASK(32));


 //   REG32(CMAC_IOBASE+CMAC_RDSAR)=VA2PA(CMACRxdescStartAddr);
    //Interrupt Configuration
    tmp=RTL_R16(CMAC_IMR);
    RTL_W16(CMAC_IMR, 0xFFFC&tmp);
    tmp=RTL_R16(CMAC_ISR);
    RTL_W16(CMAC_ISR, 0x0003|tmp);//clear
    tmp=RTL_R16(CMAC_IMR);
    RTL_W16(CMAC_IMR,0x0003|tmp);
    tp->hwrxptr_cmac=0;
    //Enable RX
    RTL_W8(CMAC_OCR0, 0x01);
}

void bsp_disable_cmac_tx(struct rtl8117_cmac_controller *tp)
{
	void __iomem *ioaddr = tp->cmac_addr;
    //INT16U tmp;
    u8 tmp1;

    //tmp=REG16(CMAC_IOBASE+CMAC_IMR);
    //REG16(CMAC_IOBASE+CMAC_IMR)=0xFFF3&tmp;
    //Disable TXa
printk(KERN_ERR "[CMAC] %s: Rx \n", __func__);
    tmp1= RTL_R8(CMAC_OCR2);
    tmp1&=0xFE;
    RTL_W8(CMAC_OCR2, tmp1);
    tp->hwtxptr_cmac=0;
}

void bsp_enable_cmac_tx(struct rtl8117_cmac_controller *tp)
{
    u8 i;
    u16 tmp=0;
	void __iomem *ioaddr = tp->cmac_addr;
        void *data;

    //Tx desciptor setup
/*	for (i = 0; i < CMAC_NUM_TX_DESC; i++) {
		if(i ==(CMAC_NUM_TX_DESC-1))
			tp->CMAC_TxDescArray[i].opts1 = 0x70000000;
		else
			tp->CMAC_TxDescArray[i].opts1 = 0x30000000;

		tp->CMAC_TxDescArray[i].opts2 = 0;
		tp->CMAC_TxDescArray[i].addr = 0;
	}
*/
	printk(KERN_ERR "[CMAC] %s: Tx \n", __func__);
        for(i=0; i<CMAC_NUM_TX_DESC; i++){
                //void *data;
                if(tp->Tx_databuff[i]){
                        cmac_mark_to_driver(tp->CMAC_TxDescArray + i, rx_buf_sz);
                        printk(KERN_ERR "[CMAC] %s: reset tx \n", __func__);
                }else{
                        data = cmac_alloc_tx_data(tp, tp->CMAC_TxDescArray + i);
                        if (!data) {
                                cmac_make_unusable_by_driver(tp->CMAC_TxDescArray + i);
                                dev_err(&tp->pdev->dev, "can't allocate tx buffer memory\n");
                        }
                        printk(KERN_ERR "[CMAC] %s: alloc tx \n", __func__);
                        tp->Tx_databuff[i] = data;
                }
        }

	cmac_mark_as_last_descriptor((struct RxDesc *)tp->CMAC_TxDescArray + CMAC_NUM_TX_DESC - 1);

//    REG32(CMAC_IOBASE+CMAC_TNPDS)=VA2PA(CMACTxdescStartAddr);
//	RTL_W32(TxDescStartAddrLow, ((u64) tp->TxPhyAddr) & DMA_BIT_MASK(32));
	RTL_W32(CMAC_TNPDS, ((u64) tp->CMAC_TxPhyAddr) & DMA_BIT_MASK(32)); 
    //Interrupt Configuration
    tmp=RTL_R16(CMAC_IMR);
    RTL_W16(CMAC_IMR, 0xFFF3&tmp);
    tmp=RTL_R16(CMAC_ISR);
    RTL_W16(CMAC_ISR, 0x000c|tmp);//clear
    tmp=RTL_R16(CMAC_IMR);
    RTL_W16(CMAC_IMR, 0x000c|tmp);
    tp->hwtxptr_cmac=0;
    while(!(RTL_R16(CMAC_ISR)&DWBIT05))
    {
	printk(KERN_ERR "[CMAC] %s: hang \n", __func__);
    }

    //Enable TX
    RTL_W8(CMAC_OCR2, 0x01);
}


void rtl8117_cmac_send(struct rtl8117_cmac_controller *tp, char *data, u32 size)
{
	void __iomem *ioaddr = tp->cmac_addr;
struct device *d = &tp->pdev->dev;
//dma_addr_t mapping;
    u32 i = 0;
    CMACTxDesc *txd_cmac;
void *txdata = tp->Tx_databuff[tp->hwtxptr_cmac % CMAC_NUM_TX_DESC];
dma_addr_t addr;
//void *alloc_data;
//int node = -1;
	unsigned int entry = tp->hwtxptr_cmac % CMAC_NUM_TX_DESC;
	//struct CMACTxDesc *txd_cmac = (struct CMACTxDesc *)tp->CMAC_TxDescArray[entry];


    txd_cmac=(CMACTxDesc *) tp->CMAC_TxDescArray + entry;

    while(txd_cmac->OWN==1)
    {
        i++;
        if(i == 100)
        {
            printk("bsp_cmac_send own bit not turn\n");
        }
    }
/*
	alloc_data = kmalloc_node(rx_buf_sz, GFP_KERNEL, node);
        if (!alloc_data)
                return;

        if (cmac_align(alloc_data) != alloc_data) {
                kfree(alloc_data);
                alloc_data = kmalloc_node(rx_buf_sz + 15, GFP_KERNEL, node);
                if (!alloc_data)
                        return;
        }

	memcpy(cmac_align(alloc_data), data, size);
	
        mapping = dma_map_single(d, cmac_align(alloc_data), rx_buf_sz,
                                 DMA_TO_DEVICE);
*/
/*         dma_addr_t addr;
                        int pkt_size;

                        addr = rxd_cmac->BufferAddress;
                        pkt_size = rxd_cmac->Length;

                        data = cmac_align(data);
                        dma_sync_single_for_cpu(d, addr, pkt_size, DMA_FROM_DEVICE);

                    rddataptr=(u8 *)data;

                    hdr = (OSOOBHdr *)rddataptr;
                        printk("bsp_cmac_handler_sw hdr->hostReqV =  %x \n", hdr->hostReqV );
                        if(DASH_ib_write_length ==0)
                        {
                                int i;
                            printk("bsp_cmac_handler_sw  copy rx %x \n", rxd_cmac->Length);
                            memcpy(tp->DASH_ib_write_buffer, rddataptr, rxd_cmac->Length);
*/
        addr = txd_cmac->BufferAddress;

//dma_sync_single_for_cpu(d, addr, size, DMA_TO_DEVICE);

	memcpy(txdata, data, size);

dma_sync_single_for_device(d, addr, size, DMA_TO_DEVICE);



       // mapping = dma_map_single(d, addr, rx_buf_sz,
       //                          DMA_TO_DEVICE);

//txd_cmac->BufferAddress= cpu_to_le64(VA2PA(mapping));
//    memcpy((INT8U *)VA2PA(CMACTXBUFFER), data, size);
//    mapping = dma_map_single(d, data, size, DMA_TO_DEVICE);

//txd_cmac->BufferAddress= cpu_to_le64(VA2PA(mapping));
	wmb();
    txd_cmac->TAGC=0;
    txd_cmac->Length=size;
    txd_cmac->FS=1;
    txd_cmac->LS=1;
    //txd_cmac->BufferAddress= cpu_to_le64(VA2PA(CMACTXBUFFER));
    txd_cmac->OWN=1;


	wmb();

    RTL_W8(CMAC_OCR2,(0x02|RTL_R8(CMAC_OCR2)));
    mmiowb();	
    tp->hwtxptr_cmac=tp->hwtxptr_cmac+1;

}


void bsp_cmac_reset(struct rtl8117_cmac_controller *tp)
{
	void __iomem *ioaddr = tp->cmac_addr;
    if(tp->CMAC_TX_Need_Disable)
    {
        bsp_disable_cmac_tx(tp);
        while(!(RTL_R16(CMAC_ISR)&DWBIT05))
        {
	printk(KERN_ERR "[CMAC] %s: Tx \n", __func__);
        }
        bsp_enable_cmac_tx(tp);
        tp->CMAC_TX_Need_Disable = 0;
    }

    if(tp->CMAC_RX_Need_Disable)
    {
        bsp_disable_cmac_tx(tp);
        while(!(RTL_R16(CMAC_ISR)&DWBIT05))
        {
	printk(KERN_ERR "[CMAC] %s: Rx \n", __func__);
        }

        bsp_disable_cmac_rx(tp);
        bsp_enable_cmac_rx(tp);
        bsp_enable_cmac_tx(tp);
        tp->CMAC_RX_Need_Disable = 0;
    }
}

irqreturn_t rtl8117_cmac_intr(int irq, void *dev_instance)
{
	struct rtl8117_cmac_controller *tp = dev_instance;
	void __iomem *ioaddr = tp->cmac_addr;
    int handled = 0;
    volatile u16 val = 0;
    u8 i = 0;
    u8 *rddataptr;
    //volatile INT16U		IMR_CMAC;
    OSOOBHdr *hdr;
    //char dbg[64];
  CMACRxDesc *rxd_cmac;


    //IMR_CMAC = REG16(CMAC_IOBASE+CMAC_IMR);
    RTL_W16(CMAC_IMR, 0x0000);
    val=RTL_R16(CMAC_ISR);
    RTL_W16(CMAC_ISR,val);//write 1 to any bit in the ISR will reset that bit
    do
    {
        printk("bsp_cmac_handler_sw %x \n", val);
        //socket_write(0, dbg, strlen(dbg));

        if((val&(DWBIT00|DWBIT01))!=0)//ROK & RDU
        {
            if(val&DWBIT01)//RDU
            {

            }
		
            for(i=0; i<CMAC_NUM_RX_DESC; i++)
            {
                
		rmb();
		rxd_cmac=(CMACRxDesc *)tp->CMAC_RxDescArray + tp->hwrxptr_cmac;

		//rmb();

                if(rxd_cmac->OWN==1)
                {
                    break;
                }
                else
                {
			struct device *d = &tp->pdev->dev;
			void *data = tp->Rx_databuff[tp->hwrxptr_cmac];
			dma_addr_t addr;
			int pkt_size;
			
			addr = rxd_cmac->BufferAddress;
			pkt_size = rxd_cmac->Length;

			data = cmac_align(data);
			dma_sync_single_for_cpu(d, addr, pkt_size, DMA_FROM_DEVICE);

                    rddataptr=(u8 *)data;

                    hdr = (OSOOBHdr *)rddataptr;
			printk("bsp_cmac_handler_sw hdr->hostReqV =  %x \n", hdr->hostReqV );		
                        if(DASH_ib_write_length ==0)
                        {
				int i;
		            printk("bsp_cmac_handler_sw  copy rx %x \n", rxd_cmac->Length);
                            memcpy(tp->DASH_ib_write_buffer, rddataptr, rxd_cmac->Length);
			    DASH_ib_write_length= rxd_cmac->Length;

				for (i=0; i<rxd_cmac->Length;i++)
		                 printk(KERN_ALERT "%x_%x\n", *(rddataptr+i),tp->DASH_ib_write_buffer[i])		;
                        }
               //         OSSemPost(DASH_OS_Response_Event);

			dma_sync_single_for_device(d, addr, pkt_size, DMA_FROM_DEVICE);

                   // memset(dbg, 0, 64);
                    //sprintf(dbg, "hdr->hostReqV %x \n", hdr->hostReqV);
                    //socket_write(0, dbg, strlen(dbg));
                    //
                    //swfun(rddataptr, rxd_cmac->Length);
                }
                //Release RX descriptor
                rxd_cmac->OWN=1;
                //Update descriptor pointer
                tp->hwrxptr_cmac=(tp->hwrxptr_cmac+1)%CMAC_NUM_RX_DESC;
            }
		kobject_uevent(&tp->pdev->dev.kobj, KOBJ_CMAC_RX);
		printk("bsp_cmac_handler_sw  KOBJ_CMAC_RX  uevent \n");

        }


        if(val & DWBIT02)//TOK
        {
            //OSSemPost(DASHREQLOCK);
        }

        if(val & DWBIT03)//TDU
        {
		//RTL_W8(CMAC_OCR2,(0x02|RTL_R8(CMAC_OCR2)));
        }

        //printf("CMAC ISR:%d\n", val);

        if(val & DWBIT06)
        {
            tp->CMAC_TX_Need_Disable=1;
        }

        if(val & DWBIT07)
        {

            tp->CMAC_RX_Need_Disable=1;
        }

        val=RTL_R16(CMAC_ISR);
        RTL_W16(CMAC_ISR, val);//write 1 to any bit in the ISR will reset that bit
	
	printk("bsp_cmac_handler_sw %x \n", val);
	handled=1;
    } while((val&0x4F)!=0);

    bsp_cmac_reset(tp);

    RTL_W16(CMAC_IMR, 0x4f);

	return IRQ_RETVAL(handled);
}

void rtl8117_cmac_device_init(struct rtl8117_cmac_controller *tp)
{
	struct platform_device *pdev = tp->pdev;
	void __iomem *ioaddr = tp->cmac_addr;
	u16 tmp;
	
	tp->CMAC_TxDescArray = dma_alloc_coherent(&pdev->dev, CMAC_TX_RING_BYTES,
						 &tp->CMAC_TxPhyAddr, GFP_KERNEL);

	tp->CMAC_RxDescArray = dma_alloc_coherent(&pdev->dev, CMAC_RX_RING_BYTES,
						 &tp->CMAC_RxPhyAddr, GFP_KERNEL);

	
	//Disable RX
	RTL_W8(CMAC_OCR0, 0x00);
	//Disable TX
	RTL_W8(CMAC_OCR2, 0x00);

    //reset and enable
    //bit6 : rx_disable_status
    //bit7 : pcie_reset
	tmp=RTL_R16(CMAC_IMR);
	RTL_W16(CMAC_IMR, 0xFF3F&tmp);

	tmp=RTL_R16(CMAC_ISR);
	RTL_W16(CMAC_ISR, 0x00C0|tmp);//clear

	tmp=RTL_R16(CMAC_IMR);
	RTL_W16(CMAC_IMR, 0x00C0|tmp);

	bsp_enable_cmac_tx(tp);
	bsp_enable_cmac_rx(tp);

	//reset simulation status bit
    	RTL_W8(CMAC_OCR3, 0);

	ioaddr = tp->oobmac_addr;
	//handshaking polling bit
	RTL_W32(0x0180, 0x00000000);

}


void cmac_intr_stop(struct rtl8117_cmac_controller *tp)
{
	 void __iomem *ioaddr = tp->kvm_addr;
//	ioaddr = tp->oobmac_addr;

	RTL_W32(0x20, 0x25);
	printk("[CMAC] Cmac interrupt stop \n");
	ioaddr = tp->oobmac_addr;
	RTL_W32(0x0108, 1);
}


void cmac_intr_init(struct rtl8117_cmac_controller *tp)
{
	void __iomem *ioaddr = tp->cmac_addr;
	u16 tmp;
	
printk("[CMAC] Cmac interrupt init \n");
	   //Disable RX
        RTL_W8(CMAC_OCR0, 0x00);
        //Disable TX
        RTL_W8(CMAC_OCR2, 0x00);

    //reset and enable
    //bit6 : rx_disable_status
    //bit7 : pcie_reset
        tmp=RTL_R16(CMAC_IMR);
        RTL_W16(CMAC_IMR, 0xFF3F&tmp);

        tmp=RTL_R16(CMAC_ISR);
        RTL_W16(CMAC_ISR, 0x00C0|tmp);//clear

        tmp=RTL_R16(CMAC_IMR);
        RTL_W16(CMAC_IMR, 0x00C0|tmp);

        bsp_enable_cmac_tx(tp);
        bsp_enable_cmac_rx(tp);

        //reset simulation status bit
        RTL_W8(CMAC_OCR3, 0);

	ioaddr = tp->kvm_addr;
	RTL_W32(0x20, 0x26);

        ioaddr = tp->oobmac_addr;
	RTL_W32(0x0108, 1);
}

static int rtl8117_cmac_open(struct inode *inode, struct file *file)
{
        return single_open(file, NULL, PDE_DATA(inode));
}

static ssize_t rtl8117_cmac_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
        ssize_t len = 0;
        char str[1000];
	//u8 val = 1;
	int i, n, max = 0x40;
	struct rtl8117_cmac_controller *tp =  ((struct seq_file *)file->private_data)->private;
	u32 dword_rd;
	void __iomem *ioaddr = tp->cmac_addr;

        len = sprintf(str, "\nDump CMAC Registers\n");
        len += sprintf(str+len, "Offset\tValue\n------\t-----");

                       for (n = 0; n < max;) {
                                len += sprintf(str+len, "\n0x%02x:\t", n);
                                for (i = 0; i < 16 && n < max; i+=4, n+=4) {
                                        dword_rd = RTL_R32(n);
                                        len += sprintf(str+len, "%08x ", dword_rd);
                                 }
                        }
	len += sprintf(str+len,"\n");

	len += sprintf(str+len, "\nDump Tx Descriptor\n");

        for (i = 0; i < CMAC_NUM_TX_DESC; i++) {
                len += sprintf(str+len, "Tx Desc %02x ", i);
                len += sprintf(str+len, "status1=%x ", le32_to_cpu(tp->CMAC_TxDescArray[i].opts1));
                len += sprintf(str+len, "status2=%x ", le32_to_cpu(tp->CMAC_TxDescArray[i].opts2));
                len += sprintf(str+len, "addr=%lx !\n", (long unsigned int)le64_to_cpu(tp->CMAC_TxDescArray[i].addr));
        }

        len += sprintf(str+len, "\nDump Rx Descriptor\n");

        for (i = 0; i < CMAC_NUM_RX_DESC; i++) {
                len += sprintf(str+len, "Rx Desc %02x ", i);
                len += sprintf(str+len, "status1=%x ", le32_to_cpu(tp->CMAC_RxDescArray[i].opts1));
                len += sprintf(str+len, "status2=%x ", le32_to_cpu(tp->CMAC_RxDescArray[i].opts2));
                len += sprintf(str+len, "addr=%lx !\n", (long unsigned int)le64_to_cpu(tp->CMAC_RxDescArray[i].addr));
        }


        copy_to_user(buf, str, len);
        if (*ppos == 0)
                *ppos += len;
        else
                len = 0;

        return len;
}

static ssize_t rtl8117_cmac_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
        char tmp[32];
        u8 num;

        struct rtl8117_cmac_controller *tp =  ((struct seq_file *)file->private_data)->private;
        void __iomem *ioaddr = tp->cmac_addr;
        u32 lanwake,lanwake2;
 
	 if (buf && !copy_from_user(tmp, buf, sizeof(tmp))) {
                num = sscanf(tmp, "%x %x", &lanwake, &lanwake2);
                if ((lanwake == 1)) {
		       printk(KERN_INFO "\n cmac register interrupt to gmac\n");
		 //       printk(KERN_INFO "Offset\tValue\n------\t-----\n");
	
			register_swisr(0x25,(int (*)(void *))cmac_intr_stop,tp);
			register_swisr(0x26,(int (*)(void *))cmac_intr_init,tp);
		}
		 else{
		 	RTL_W32(lanwake, lanwake2);
                        printk(KERN_INFO "write procfs not support value  = %x %x\n", lanwake, lanwake2);

		}
        }

        printk(KERN_INFO "[CMAC] set echi to cxxx\n" );
        return count;
}

long rtl8117_cmac_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct rtl8117_cmac_controller *tp =  ((struct seq_file *)file->private_data)->private;
	struct rtl_dash_ioctl_struct data;
	int i;
	long retval=0;
 
	memset(&data, 0, sizeof(data));

	printk(KERN_ALERT "IOCTL enter......\n");

	if (_IOC_TYPE(cmd) != RTLOOB_IOC_MAGIC)
        {
        //        DbgFunPrint("Invalid command!!!");
                return -ENOTTY;
        }

	switch (cmd) {
	case IOCTL_SEND:
	   if (copy_from_user(&data, (int __user *)arg, sizeof(data))) {
		    retval = -EFAULT;
		    goto done;
	   }
	   printk(KERN_ALERT "IOCTL send val:%x .\n", data.len);

	   rtl8117_cmac_send(tp, data.data_buffer2, data.len);
           for (i=0; i<data.len;i++)
                printk("%02x ",*(data.data_buffer2+i));
	
		break;
	case IOCTL_RECV:
	   if (copy_from_user(&data, (int __user *)arg, sizeof(data))) {
                    retval = -EFAULT;
                    goto done;
           }	

	     data.len = DASH_ib_write_length;

	     for (i=0; i<data.len;i++){
                data.data_buffer[i]=i+10;
		//*(data.data_buffer2+i)=i+10;
		}
	     //memcpy(data.data_buffer2, data.data_buffer, data.len);	
             memcpy(data.data_buffer2, tp->DASH_ib_write_buffer, DASH_ib_write_length);
	     if (copy_to_user((int __user *)arg, &data, sizeof(data)) ) {
                                retval = -EFAULT;
                                goto done;
              }
	     printk(KERN_ALERT "IOCTL recv val:%x .\n", data.len);	

	DASH_ib_write_length=0;

	    for (i=0; i<data.len;i++)
		 printk(KERN_ALERT "%x-%x\n", data.data_buffer[i],*(data.data_buffer2+i));
		//:wq printk(KERN_ALERT "%x\n", data.data_buffer[i]);
                
	      break;	
	default:
	   retval = -ENOTTY;

	}
done:
	return retval;
}


static const struct file_operations cmac_fops =
{
        .owner = THIS_MODULE,
        .open = rtl8117_cmac_open,
        .read = rtl8117_cmac_read,
        .write = rtl8117_cmac_write,
	.unlocked_ioctl = rtl8117_cmac_ioctl,
};

static int rtl8117_cmac_probe(struct platform_device *pdev)
{
	int retval;
	struct rtl8117_cmac_controller *tp;

	printk(KERN_ERR "[CMAC] %s: gpio is not valid\n", __func__);


	tp = kzalloc(sizeof(struct rtl8117_cmac_controller), GFP_KERNEL);
	if (!tp)
        {
                dev_err(&pdev->dev, "rtl8117 cmac: failed to allocate device structure.\n");
                return -ENOMEM;
        }
	memset(tp, 0, sizeof(struct rtl8117_cmac_controller));

	tp->pdev = pdev;
	
	tp->cmac_addr = of_iomap(pdev->dev.of_node, 0);
        if (!tp->cmac_addr) {
                dev_err(&pdev->dev, "can't request CMAC base address\n");
                return -EINVAL;
        }
	
	tp->oobmac_addr = of_iomap(pdev->dev.of_node, 1);
        if (!tp->oobmac_addr) {
                dev_err(&pdev->dev, "can't request OOBMAC base address\n");
                return -EINVAL;
        }
	 
        tp->kvm_addr = of_iomap(pdev->dev.of_node, 2);
        if (!tp->oobmac_addr) {
                dev_err(&pdev->dev, "can't request KVM base address\n");
                return -EINVAL;
        }

	tp->irq = irq_of_parse_and_map(pdev->dev.of_node, 0);
        if (tp->irq < 0) {
                dev_err(&pdev->dev, "missing IRQ resource\n");
                return tp->irq;
        }


	platform_set_drvdata(pdev, tp);

	rtl8117_cmac_device_init(tp);


        proc_cmac_dir = proc_mkdir(dir_name, NULL);
        if (!proc_cmac_dir)
        {
                dev_err(&pdev->dev,"Create directory \"%s\" failed.\n", dir_name);
                return -1;
        }

        proc_cmac_entry = proc_create_data(entry_name, 0666, proc_cmac_dir, &cmac_fops, tp);
        if (!proc_cmac_entry)
        {
                dev_err(&pdev->dev, "Create file \"%s\"\" failed.\n", entry_name);
                return -1;
        }

	retval = devm_request_irq(&pdev->dev, tp->irq, rtl8117_cmac_intr, IRQF_SHARED,
                                  dev_name(&pdev->dev), tp);
        if (retval) {
                dev_err(&pdev->dev, "can't request irq\n");
                return retval;
        }

/*
	    INT16U tmp=0;

    CMAC_SW_init();

    //Disable RX
    REG8(CMAC_IOBASE+CMAC_OCR0)=0x00;
    //Disable TX
    REG8(CMAC_IOBASE+CMAC_OCR2)=0x00;

    //reset and enable
    //bit6 : rx_disable_status
    //bit7 : pcie_reset
    tmp=REG16(CMAC_IOBASE+CMAC_IMR);
    REG16(CMAC_IOBASE+CMAC_IMR)=0xFF3F&tmp;
    tmp=REG16(CMAC_IOBASE+CMAC_ISR);
    REG16(CMAC_IOBASE+CMAC_ISR)=0x00C0|tmp;//clear
    tmp=REG16(CMAC_IOBASE+CMAC_IMR);
    REG16(CMAC_IOBASE+CMAC_IMR)=0x00C0|tmp;

    bsp_enable_cmac_tx();

    bsp_enable_cmac_rx();


    //reset simulation status bit
    REG8(CMAC_IOBASE+CMAC_OCR3)=0;

    //handshaking polling bit
    REG32(IOREG_IOBASE+0x00000180)= 0x00000000;
    rlx_irq_register(BSP_CMAC_IRQ, bsp_cmac_handler_sw_patch);
*/
	return 0;
}

static const struct of_device_id rtl8117_cmac_ids[] = {
	{ .compatible = "realtek,rtl8117-cmac" },
	{},
};
MODULE_DEVICE_TABLE(of, rtl8117_cmac_ids);

int rtl8117_cmac_remove(struct platform_device *pdev)
{
	return 0;
}

static struct platform_driver rtl8117_cmac_driver = {
	.probe = rtl8117_cmac_probe,
	.remove = rtl8117_cmac_remove,
	.driver = {
		.name = MODULENAME,
		.of_match_table = of_match_ptr(rtl8117_cmac_ids),
	},
};

module_platform_driver(rtl8117_cmac_driver);

MODULE_AUTHOR("Ted Chen <tedchen@realtek.com>");
MODULE_DESCRIPTION("Realtek RTL8117 cmac driver");
MODULE_LICENSE("GPL");

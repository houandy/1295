
typedef struct _CMACRxDesc {
    u32		Length:14;
    u32		TCPF:1;
    u32		UDPF:1;
    u32		IPF:1;
    u32		PKTFNO:4;
    u32		TCPT:1;
    u32		UDPT:1;
    u32		V4F:1;
    u32		V6F:1;
    u32		BRO:1;
    u32		PHY:1;
    u32		MAR:1;
    u32		LS:1;
    u32		FS:1;
    u32		EOR:1;
    u32		OWN:1;
//----------
    u32		VLanTag:16;
    u32		TAVA:1;
    u32		RSVD:14;
    u32		GAME:1;
//----------
    u32		BufferAddress;
    u32		BufferAddressHigh;
} CMACRxDesc; //volatile
//--------------------
typedef struct _CMACTxDesc {
    u32		Length:16;
    u32		RSVD0:9;
    u32		GTSENV6:1;
    u32		GTSENV4:1;
    u32		RSVD1:1;
    u32		LS:1;
    u32		FS:1;
    u32		EOR:1;
    u32		OWN:1;
//----------
    u32		VLanTag:16;
    u32		LGSEN:1;
    u32		TAGC:1;
    u32		TCPHO:10;
    u32		V6F:1;
    u32		IPV4CS:1;
    u32		TCPCS:1;
    u32		UDPCS:1;
//----------
    u32		BufferAddress;
    u32		BufferAddressHigh;
} CMACTxDesc; //volatile


typedef struct _OSOOBHdr {
    unsigned int len;
    unsigned char type;
    unsigned char flag;
    unsigned char hostReqV;
    unsigned char res;
} OSOOBHdr;

struct TxDesc {
	__le32 opts1;
	__le32 opts2;
	__le64 addr;
};

struct RxDesc {
	__le32 opts1;
	__le32 opts2;
	__le64 addr;
};

#define DWBIT00 	0x00000001
#define DWBIT01 	0x00000002
#define DWBIT02 	0x00000004
#define DWBIT03 	0x00000008
#define DWBIT04 	0x00000010
#define DWBIT05 	0x00000020
#define DWBIT06 	0x00000040
#define DWBIT07 	0x00000080
#define DWBIT08 	0x00000100
#define DWBIT09 	0x00000200
#define DWBIT10 	0x00000400
#define DWBIT11 	0x00000800
#define DWBIT12 	0x00001000
#define DWBIT13 	0x00002000
#define DWBIT14 	0x00004000
#define DWBIT15 	0x00008000
#define DWBIT16 	0x00010000
#define DWBIT17 	0x00020000
#define DWBIT18 	0x00040000
#define DWBIT19 	0x00080000
#define DWBIT20 	0x00100000
#define DWBIT21 	0x00200000
#define DWBIT22 	0x00400000
#define DWBIT23 	0x00800000
#define DWBIT24 	0x01000000
#define DWBIT25 	0x02000000
#define DWBIT26 	0x04000000
#define DWBIT27 	0x08000000
#define DWBIT28 	0x10000000
#define DWBIT29 	0x20000000
#define DWBIT30 	0x40000000
#define DWBIT31 	0x80000000

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

#define CMAC_NUM_TX_DESC		4 //Max.=16
#define CMAC_NUM_RX_DESC		4 //Max.=16

#define CMAC_TX_RING_BYTES	(CMAC_NUM_TX_DESC * sizeof(struct TxDesc))
#define CMAC_RX_RING_BYTES	(CMAC_NUM_RX_DESC * sizeof(struct RxDesc))

#define CMAC_simulation_case1	1//CMAC RX check incremental data
#define CMAC_simulation_case2	0//CMAC TX send incremental data
#define CMAC_simulation_case3	0//CMAC TX send back data from CMAC RX

#define handshaking_chk		0

struct rtl_dash_ioctl_struct {
        __u32	cmd;
        __u32	offset;
        __u32	len;
        __u8 data_buffer[100];
	__u8 *data_buffer2;
};


#define RTLOOB_IOC_MAGIC 0x96

/* 您要的動作 */
#define IOCTL_SEND      _IOW(RTLOOB_IOC_MAGIC, 0, struct rtl_dash_ioctl_struct)
#define IOCTL_RECV      _IOR(RTLOOB_IOC_MAGIC, 1, struct rtl_dash_ioctl_struct)

void bsp_cmac_handler(void);
void bsp_cmac_init(void);
void CMAC_SW_release(void);
void bsp_cmac_send(char *data, u32 size);
void bsp_cma_TestRegisterRW(void);
//void bsp_cmac_handler_sw_patch(void);
//void bsp_cmac_reset(void);

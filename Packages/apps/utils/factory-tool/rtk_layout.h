#ifndef RTK_LAYOUT_H
#define RTK_LAYOUT_H

#ifdef STOR_TYPE_EMMC

#define ENV_TOTAL_SIZE  131072
#ifdef CHIP_TYPE_THOR
    #define FACTORY_START_ADDR  0x621000
    #define FACTORY_SIZE        0x400000
#elif CHIP_TYPE_KYLIN
    #define FACTORY_START_ADDR  0x220000
    #define FACTORY_SIZE        0x400000
#endif /* CHIP_TYPE */

#elif STOR_TYPE_SPI

#define ENV_TOTAL_SIZE  8192
#ifdef CHIP_TYPE_THOR
    #define FACTORY_START_ADDR  0x0
    #define FACTORY_SIZE        0x20000
#elif CHIP_TYPE_KYLIN
    #define FACTORY_START_ADDR  0x0
    #define FACTORY_SIZE        0x20000
#endif /* CHIP_TYPE */

#else
/* define default value to avoid compiler complaint, should never get here */
#define ENV_TOTAL_SIZE      131072
#define FACTORY_START_ADDR  0x110000
#define FACTORY_SIZE        0x20000
#endif /* STOR_TYPE */


#endif /* RTK_LAYOUT_H */

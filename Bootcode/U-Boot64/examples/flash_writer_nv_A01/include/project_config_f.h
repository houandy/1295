/************************************************************************
 *
 *  project_config.h
 *
 *  external parameters was included in this file
 *
 *
 ************************************************************************/

#ifndef PROJECT_CONFIG_H
#define PROJECT_CONFIG_H
//********************************************************************
//Board Components
//********************************************************************
//flag                      value
//********************************************************************
#define Board_CPU_RTD161x
#define Board_Chip_Rev_161x
#define Board_HWSETTING_RTD161x_hwsetting_BOOT_2DDR4_8Gb_s2666
//********************************************************************
//Thor Sign Keys
//********************************************************************
//********************************************************************
//NOCS Certificate and Params Attributes
//********************************************************************
//NOTICE start: Please don't change the format of variables as below, otherwise, you have to change the build_fig.sh
//NOTICE end
//TEE Protect Region
//AVFW_TEXT Protect Region
//AVFW_DATA Protect Region
//AVFW_ISR Protect Region
//AVFW_ENTRY Protect Region
//********************************************************************
//Config for Security, TRUE will encrypt the DTE_Bootcode_RTK(LK or uboot64)
//********************************************************************
#define Config_Secure_Improve_FALSE
//********************************************************************
//Config for Simulation Mode - only use BOOTCODE_UBOOT_TARGET_SIM, don't ignore RSA calculation
//********************************************************************
//flag                      value
//********************************************************************
#define Config_Uboot_Sim_Mode_FALSE
//********************************************************************
//Config for Auxcode/BL31/TEE OS
//********************************************************************
//flag                      value
//********************************************************************
#define Config_Auxcode_File_Name			"Aux_code-00___00.bin"
#define Config_PCPU_Code_File_Name			"pcpu_fw___bin"
#define Config_DTE_Bootcode_File_Name			"FSBL_DDR-00___00.bin"
#define Config_SECURE_OS_FALSE
#define Config_SECURE_OS_File_Name			"secure-os-00___00.bin"
#define Config_BL31_TRUE
#define Config_BL31_File_Name			"bl31___bin"
//********************************************************************
//Boot parameters
//********************************************************************
//flag                      value
//********************************************************************
#define Param_MAC_hi			0x00112233
#define Param_MAC_lo			0x44550000
//********************************************************************
//user defined
//********************************************************************
//flag                      value
//********************************************************************
#define RTK_FLASH_EMMC "1"

#endif //#ifndef EXTERN_PARAM_H

#!/bin/bash
#set -x

#rm -rf DVRBOOT_OUT
#[ $# = 0 ] && echo -e "Usage:\n\t./build.sh [RTD16xx_spi|RTD16xx_emmc|RTD12xx_emmc]\n\t./build.sh\t=>For building all version"
[ $# = 0 ] && echo -e "Usage:\n\t./build.sh [RTD16xx_spi|RTD16xx_emmc]" && exit 0
target=$1

########### Build Thor A00 RTK #############
#BUILD_HWSETTING_LIST=`ls $HWSETTING_DIR`
#BUILD_HWSETTING_LIST=RTD161x_hwsetting_BOOT_2DDR4_8Gb_s1600 RTD161x_hwsetting_BOOT_2DDR4_8Gb_s2400 RTD161x_hwsetting_BOOT_2DDR4_8Gb_s2133 RTD161x_hwsetting_BOOT_2DDR4_8Gb_s2666 RTD161x_hwsetting_BOOT_LPDDR4_16Gb_s3200_H 

if [ $target = RTD16xx_spi ]; then
	CHIP_TYPE=0001
	HWSETTING_DIR=examples/flash_writer_nv_A01/hw_setting/rtd161x/nas/$CHIP_TYPE
	BUILD_HWSETTING_LIST=RTD161x_hwsetting_BOOT_2DDR4_8Gb_s2666
	make mrproper; make rtd161x_spi_nas_rtk_defconfig;


	for hwsetting in $BUILD_HWSETTING_LIST
	do
		hwsetting=`echo $hwsetting | cut -d '.' -f 1`
		echo %%%%%%%% RTD1619 -- $CHIP_TYPE -- $hwsetting %%%%%%
		if [[ $hwsetting == *"NAND"* ]]; then
			echo "NAND hwsetting skip"
			continue
		fi
	
		#Build the normal version
		make Board_HWSETTING=$hwsetting CHIP_TYPE=$CHIP_TYPE
		rm -rf ./DVRBOOT_OUT/$target; mkdir -p ./DVRBOOT_OUT/$target/hw_setting 
		cp ./examples/flash_writer_nv_A01/hw_setting/out/${hwsetting}_final.bin ./DVRBOOT_OUT/$target/hw_setting/$CHIP_TYPE-$hwsetting-nas-$target.bin
		cp ./examples/flash_writer_nv_A01/dvrboot.exe.bin ./DVRBOOT_OUT/$target/A01-$hwsetting-nas-$target.bin
		cp ./examples/flash_writer_nv_A01/Bind/bind.bin ./DVRBOOT_OUT/$target/A01-Recovery-$hwsetting-nas-$target.bin
	done
fi



if [ $target = RTD16xx_emmc ]; then
	CHIP_TYPE=0001
	HWSETTING_DIR=examples/flash_writer_nv_A01/hw_setting/rtd161x/nas/$CHIP_TYPE
	BUILD_HWSETTING_LIST=RTD161x_hwsetting_BOOT_2DDR4_8Gb_s2666
	make mrproper; make rtd161x_qa_nas_rtk_defconfig;

	for hwsetting in $BUILD_HWSETTING_LIST
	do
		hwsetting=`echo $hwsetting | cut -d '.' -f 1`
		echo %%%%%%%% RTD1619 -- $CHIP_TYPE -- $hwsetting %%%%%%
		if [[ $hwsetting == *"NAND"* ]]; then
			echo "NAND hwsetting skip"
			continue
		fi

		#Build the normal version
		make Board_HWSETTING=$hwsetting CHIP_TYPE=$CHIP_TYPE
		rm -rf ./DVRBOOT_OUT/$target; mkdir -p ./DVRBOOT_OUT/$target/hw_setting 
		cp ./examples/flash_writer_nv_A01/hw_setting/out/${hwsetting}_final.bin ./DVRBOOT_OUT/$target/hw_setting/$CHIP_TYPE-$hwsetting-nas-$target.bin
		cp ./examples/flash_writer_nv_A01/dvrboot.exe.bin ./DVRBOOT_OUT/$target/A01-$hwsetting-nas-$target.bin
		cp ./examples/flash_writer_nv_A01/Bind/uda_emmc.bind.bin ./DVRBOOT_OUT/$target/A01-Recovery-uda-$hwsetting-nas-$target.bin
		cp ./examples/flash_writer_nv_A01/Bind/boot_emmc.bind.bin ./DVRBOOT_OUT/$target/A01-Recovery-boot-$hwsetting-nas-$target.bin
	done
fi



if [ $target = RTD129x_emmc ]; then
	make mrproper; make rtd1296_qa_NAS_defconfig
	make Board_HWSETTING=RTD1296_hwsetting_BOOT_4DDR4_4Gb_s1866 CONFIG_CHIP_TYPE=0002
	rm -rf ./DVRBOOT_OUT/$target; mkdir -p ./DVRBOOT_OUT/$target/hw_setting 
	cp ./examples/flash_writer/image/hw_setting/RTD1296_hwsetting_BOOT_4DDR4_4Gb_s1866.bin ./DVRBOOT_OUT/$target/hw_setting/
	cp ./examples/flash_writer/dvrboot.exe.bin ./DVRBOOT_OUT/$target/NAS_1296_B00_emmc_1866_DDR4_4X4Gb.bin
fi

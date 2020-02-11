#!/bin/bash
#set -x

#rm -rf DVRBOOT_OUT
#[ $# = 0 ] && echo -e "Usage:\n\t./build.sh [RTD16xx_spi|RTD16xx_emmc|RTD12xx_emmc]\n\t./build.sh\t=>For building all version"
[ $# -lt 2 ] && echo -e "Usage:\n\t./build.sh [RTD16xx_spi|RTD16xx_emmc|RTD129x_spi|RTD129x_emmc] [project]" && exit 0
target=$1
project=$2
now="$(date '+%Y%m%d')"
binfilefolder="DVRBOOT_OUT/${target}"
releaseimagefolder="/var/www/html/release/image/${project}"
[ -z $3 ] && VERSION_NUMBER="0.0.0" || VERSION_NUMBER=$3
[ -z $4 ] && release=0 || release=1

for patchfile in $(ls patches/*.patch)
do
        patch -p1 --no-backup-if-mismatch < ${patchfile}
done

for patchfile in $(ls patches/${project}/*.patch)
do
	patch -p1 --no-backup-if-mismatch < ${patchfile}
done

########### Build Thor A01 RTK #############
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
		make Board_HWSETTING=$hwsetting CONFIG_CHIP_TYPE=$CHIP_TYPE
		rm -rf ./DVRBOOT_OUT/$target; mkdir -p ./DVRBOOT_OUT/$target/hw_setting 
		cp ./examples/flash_writer_nv_A01/hw_setting/out/${hwsetting}_final.bin ./DVRBOOT_OUT/$target/hw_setting/$CHIP_TYPE-$hwsetting-nas-$target.bin
		cp ./examples/flash_writer_nv_A01/dvrboot.exe.bin ./DVRBOOT_OUT/$target/A01-$hwsetting-nas-$target.bin
		cp ./examples/flash_writer_nv_A01/Bind/uda_emmc.bind.bin ./DVRBOOT_OUT/$target/A01-Recovery-uda-$hwsetting-nas-$target.bin
		cp ./examples/flash_writer_nv_A01/Bind/boot_emmc.bind.bin ./DVRBOOT_OUT/$target/A01-Recovery-boot-$hwsetting-nas-$target.bin
	done
fi

if [ $target = RTD16xx_nand ]; then
	CHIP_TYPE=0001
	hwsetting=RTD161x_hwsetting_BOOT_2DDR4_8Gb_s2666
	make mrproper; make rtd161x_nand_nas_rtk_defconfig; make CONFIG_CHIP_TYPE=$CHIP_TYPE

	rm -rf ./DVRBOOT_OUT/$target; mkdir -p ./DVRBOOT_OUT/$target/hw_setting
	cp ./examples/flash_writer_nv_A01/dvrboot.exe.bin ./DVRBOOT_OUT/$target/A01-nas-$target.bin
	cp ./examples/flash_writer_nv_A01/hw_setting/out/${hwsetting}_final.bin ./DVRBOOT_OUT/$target/hw_setting/$CHIP_TYPE-$hwsetting-nas-$target.bin
	cp ./examples/flash_writer_nv_A01/Bind/nand.bind.bin ./DVRBOOT_OUT/$target/A01-Recovery-$hwsetting-nas-$target.bin
fi

# RTD1295_hwsetting_BOOT_2DDR3_4Gb_s1600
# RTD1295_hwsetting_BOOT_2DDR4_4Gb_s2133
# RTD1295_hwsetting_BOOT_2DDR3_4Gb_s1866
# RTD1295_hwsetting_BOOT_2DDR4_8Gb_s2133

# RTD1296_hwsetting_BOOT_2DDR3_4Gb_s1600
# RTD1296_hwsetting_BOOT_4DDR4_8+4Gb_s1866
# RTD1296_hwsetting_BOOT_4DDR4_4Gb_s1866
# RTD1296_hwsetting_BOOT_4DDR4_8Gb_s1866
if [ $target = RTD129x_emmc ]; then
	BUILD_HWSETTING_LIST=RTD1295_hwsetting_BOOT_2DDR4_8Gb_s2133
	rm -rf ./DVRBOOT_OUT/$target; mkdir -p ./DVRBOOT_OUT/$target/hw_setting 
	make mrproper; make rtd1295_qa_NAS_defconfig	
	for hwsetting in $BUILD_HWSETTING_LIST
	do
		make Board_HWSETTING=$hwsetting CONFIG_CHIP_TYPE=0002
		cp ./examples/flash_writer/image/hw_setting/$hwsetting.bin ./DVRBOOT_OUT/$target/hw_setting/
		cp ./examples/flash_writer/dvrboot.exe.bin ./DVRBOOT_OUT/$target/B00-$hwsetting-nas-RTD1295_emmc.bin
	done
	
	BUILD_HWSETTING_LIST=RTD1296_hwsetting_BOOT_4DDR4_4Gb_s1866
	make mrproper; make rtd1296_qa_NAS_defconfig
	for hwsetting in $BUILD_HWSETTING_LIST
	do
		make Board_HWSETTING=$hwsetting CONFIG_CHIP_TYPE=0002
		cp ./examples/flash_writer/image/hw_setting/$hwsetting.bin ./DVRBOOT_OUT/$target/hw_setting/
		cp ./examples/flash_writer/dvrboot.exe.bin ./DVRBOOT_OUT/$target/B00-$hwsetting-nas-RTD1296_emmc.bin
	done
	
	BUILD_HWSETTING_LIST=RTD1295_hwsetting_BOOT_2DDR4_8Gb_s2133
	make mrproper; make rtd1295_qa_NAS_defconfig	
	for hwsetting in $BUILD_HWSETTING_LIST
	do
		make Board_HWSETTING=$hwsetting CONFIG_CHIP_TYPE=0001
		cp ./examples/flash_writer/image/hw_setting/$hwsetting.bin ./DVRBOOT_OUT/$target/hw_setting/
		cp ./examples/flash_writer/dvrboot.exe.bin ./DVRBOOT_OUT/$target/A01-$hwsetting-nas-RTD1295_emmc.bin
	done
	
	BUILD_HWSETTING_LIST=RTD1296_hwsetting_BOOT_4DDR4_4Gb_s1866
	make mrproper; make rtd1296_qa_NAS_defconfig
	for hwsetting in $BUILD_HWSETTING_LIST
	do
		make Board_HWSETTING=$hwsetting CONFIG_CHIP_TYPE=0001
		cp ./examples/flash_writer/image/hw_setting/$hwsetting.bin ./DVRBOOT_OUT/$target/hw_setting/
		cp ./examples/flash_writer/dvrboot.exe.bin ./DVRBOOT_OUT/$target/A01-$hwsetting-nas-RTD1296_emmc.bin
	done
fi

if [ $target = RTD129x_spi ]; then
#	BUILD_HWSETTING_LIST=RTD1295_hwsetting_BOOT_2DDR4_8Gb_s2133
	rm -rf ./DVRBOOT_OUT/$target; mkdir -p ./DVRBOOT_OUT/$target/hw_setting 
#	make mrproper; make rtd1295_spi_16MB_defconfig
#	for hwsetting in $BUILD_HWSETTING_LIST
#	do
#		make Board_HWSETTING=$hwsetting CONFIG_CHIP_TYPE=0002
#		cp ./examples/flash_writer/image/hw_setting/$hwsetting.bin ./DVRBOOT_OUT/$target/hw_setting/B00-$hwsetting.bin
#		cp ./examples/flash_writer/dvrboot.exe.bin ./DVRBOOT_OUT/$target/B00-$hwsetting-nas-RTD1295_spi.bin
#	done
	
#	BUILD_HWSETTING_LIST=RTD1296_hwsetting_BOOT_4DDR4_4Gb_s1866
#	make mrproper; make rtd1296_spi_16MB_defconfig
#	for hwsetting in $BUILD_HWSETTING_LIST
#	do
#		make Board_HWSETTING=$hwsetting CONFIG_CHIP_TYPE=0002
#		cp ./examples/flash_writer/image/hw_setting/$hwsetting.bin ./DVRBOOT_OUT/$target/hw_setting/B00-$hwsetting.bin
#		cp ./examples/flash_writer/dvrboot.exe.bin ./DVRBOOT_OUT/$target/B00-$hwsetting-nas-RTD1296_spi.bin
#	done
	BUILD_HWSETTING_LIST=RTD1295_hwsetting_BOOT_2DDR4_4Gb_s2133
	make mrproper; make rtd1295_spi_16MB_defconfig
	for hwsetting in $BUILD_HWSETTING_LIST
	do
		make Board_HWSETTING=$hwsetting CONFIG_CHIP_TYPE=0001 UBOOTVERSION=${VERSION_NUMBER}_${now}
		cp ./examples/flash_writer/image/hw_setting/$hwsetting.bin ./DVRBOOT_OUT/$target/hw_setting/A01-$hwsetting.bin
		cp ./examples/flash_writer/dvrboot.exe.bin ./DVRBOOT_OUT/$target/A01-$hwsetting-nas-RTD1295_spi.bin
                cp ./DVRBOOT_OUT/$target/A01-$hwsetting-nas-RTD1295_spi.bin ./DVRBOOT_OUT/$target/Uboot-${project}.bin
                DEST_FILE="${binfilefolder}/Uboot-${project}.bin"
                DEST_HWSETTING_FILE="${binfilefolder}/hw_setting/A01-${hwsetting}.bin"
	done
	
#	BUILD_HWSETTING_LIST=RTD1296_hwsetting_BOOT_4DDR4_4Gb_s1866
#	make mrproper; make rtd1296_spi_16MB_defconfig
#	for hwsetting in $BUILD_HWSETTING_LIST
#	do
#		make Board_HWSETTING=$hwsetting CONFIG_CHIP_TYPE=0001
#		cp ./examples/flash_writer/image/hw_setting/$hwsetting.bin ./DVRBOOT_OUT/$target/hw_setting/A01-$hwsetting.bin
#		cp ./examples/flash_writer/dvrboot.exe.bin ./DVRBOOT_OUT/$target/A01-$hwsetting-nas-RTD1296_spi.bin
#	done

#	BUILD_HWSETTING_LIST=RTD1295_hwsetting_BOOT_2DDR3_4Gb_s1866
#	make mrproper; make rtd1295_spi_16MB_defconfig
#	for hwsetting in $BUILD_HWSETTING_LIST
#	do
#		make Board_HWSETTING=$hwsetting CONFIG_CHIP_TYPE=0001
#		cp ./examples/flash_writer/image/hw_setting/$hwsetting.bin ./DVRBOOT_OUT/$target/hw_setting/A01-$hwsetting.bin
#		cp ./examples/flash_writer/dvrboot.exe.bin ./DVRBOOT_OUT/$target/A01-$hwsetting-nas-RTD1295_spi.bin
#	done

fi

# RTD1295_hwsetting_BOOT_2DDR3_4Gb_s1866
# RTD1295_hwsetting_BOOT_2DDR4_8Gb_s2133
# RTD1296_hwsetting_BOOT_4DDR4_4Gb_s1866

if [ $target = RTD129x_nand ]; then
	rm -rf ./DVRBOOT_OUT/$target; mkdir -p ./DVRBOOT_OUT/$target/hw_setting 

	BUILD_HWSETTING_LIST=RTD1295_hwsetting_BOOT_2DDR3_4Gb_s1866
	make mrproper; make rtd1295_nand_NAS_defconfig
	for hwsetting in $BUILD_HWSETTING_LIST
	do
		make Board_HWSETTING=$hwsetting CONFIG_CHIP_TYPE=0001
		cp ./examples/flash_writer/image/hw_setting/$hwsetting.bin ./DVRBOOT_OUT/$target/hw_setting/A01-$hwsetting.bin
		cp ./examples/flash_writer/dvrboot.exe.bin ./DVRBOOT_OUT/$target/A01-$hwsetting-nas-RTD1295_nand.bin
	done

	BUILD_HWSETTING_LIST=RTD1295_hwsetting_BOOT_2DDR4_8Gb_s2133
	make mrproper; make rtd1295_nand_NAS_defconfig
	for hwsetting in $BUILD_HWSETTING_LIST
	do
		make Board_HWSETTING=$hwsetting CONFIG_CHIP_TYPE=0002
		cp ./examples/flash_writer/image/hw_setting/$hwsetting.bin ./DVRBOOT_OUT/$target/hw_setting/B00-$hwsetting.bin
		cp ./examples/flash_writer/dvrboot.exe.bin ./DVRBOOT_OUT/$target/B00-$hwsetting-nas-RTD1295_nand.bin
	done

	BUILD_HWSETTING_LIST=RTD1296_hwsetting_BOOT_4DDR4_4Gb_s1866
	make mrproper; make rtd1296_nand_NAS_defconfig;
	for hwsetting in $BUILD_HWSETTING_LIST
	do
		make Board_HWSETTING=$hwsetting CONFIG_CHIP_TYPE=0002
		cp ./examples/flash_writer/image/hw_setting/$hwsetting.bin ./DVRBOOT_OUT/$target/hw_setting/B00-$hwsetting.bin
		cp ./examples/flash_writer/dvrboot.exe.bin ./DVRBOOT_OUT/$target/B00-$hwsetting-nas-RTD1295_nand.bin
	done
fi

for patchfile in $(ls patches/${project}/*.patch)
do
        patch -R -p1 < ${patchfile}
done

for patchfile in $(ls patches/*.patch)
do
        patch -R -p1 < ${patchfile}
done

if [ $release == 1 ]; then
		[ -d ${releaseimagefolder}/ ] || mkdir -p ${releaseimagefolder}/
		[ -d ${releaseimagefolder}/uboot-${project}-${VERSION_NUMBER}-${now}/ ] || mkdir -p ${releaseimagefolder}/uboot-${project}-${VERSION_NUMBER}-${now}
		[ -d ${releaseimagefolder}/uboot-${project}-${VERSION_NUMBER}-${now}/ ] && {
			[ -f ${DEST_FILE} ] && cp ${DEST_FILE} ${releaseimagefolder}/uboot-${project}-${VERSION_NUMBER}-${now}/
			[ -f ${DEST_HWSETTING_FILE} ] && cp ${DEST_HWSETTING_FILE} ${releaseimagefolder}/uboot-${project}-${VERSION_NUMBER}-${now}/Uboot-${project}-hwsetting.bin
		}
fi

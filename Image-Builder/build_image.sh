#!/bin/bash
#set -x

TOPDIR=`pwd`

function msg(){
    echo -e "\033[47;31m $1  \033[0m"
}

function check_must_file(){
	if [ ! -f "$1" ]; then
		msg "$1 not found"
		exit 1
	fi
}

function check_file(){
	if [ ! -f "$1" ]; then
	#	msg "$1 not found"
		return 0
	else
		return 1
	fi
}

function check_fsdir(){
	pushd $feed_dir
	if [ ! -d "$1" ]; then
	#	msg "$1 not found"
		popd
		return 0
	else
		popd
		return 1
	fi
}

function check_dtc() {
	if [ ! $(type -t dtc) ]; then
		msg "device tree compiler is currently not installed."
		exit 1
	fi
	filesize=$(ls -la $1 | awk '{print $5}')
	if [ $filesize -eq $((0)) ]; then
		msg "size of $1 should not be zero."
		exit 1
	fi
}

function check_crc() {
	if [ ! $(type -t crc32) ]; then
		msg "crc32 is currently not installed."
		exit 1
	fi
}

function clean() {
    if [ -f $1 ]; then
        rm $1
    fi
}

function get_align_size() {
	size=$1
	if [ $((size&$((storage_align-1)))) != 0 ]; then 
		ret_get_align_size=$(($((size+storage_align))-$((size&$((storage_align-1))))))
	else
		ret_get_align_size=$size
	fi
}

function align_check() {
	get_align_size $2 
	if [ $2 != $ret_get_align_size ]; then 
		msg "$1 size $2 is unaligned($ret_get_align_size)"
		exit 1
	fi
}

function align_and_padding() {
	source=$1
	target=$2
	filesize=`ls -la $source | awk '{print $5}'`
	get_align_size $filesize; alignedsize=$ret_get_align_size
	dd if=$source of=$target bs=$alignedsize count=1 conv=sync 2> /dev/null	 
}

function copy_workspace() {
	source=$1
	target=$2

	cp $source $target
}

function padding(){
	source=$1
	target=$2
	allocsize=$3	
	filesize=`ls -la $source | awk '{print $5}'`
	get_align_size $filesize; alignedsize=$ret_get_align_size
	if [ $alignedsize -gt $allocsize ]; then
		msg "padding $source to $ target but aligned filesize($alignedsize) > allocated size ($allocsize)"
	fi

	dd if=$source of=$target bs=$alignedsize count=1 conv=sync	2> /dev/null
	return $?
}

function build_config_txt() {
	workspace=$1/$project
	output=$workspace/config.txt
	
	echo -e "#TOTALVERSION $totalversion\n" > $output
	echo -e "#VERSION $version\n" >> $output
	echo "debug_level=$debug_level" >> $output
	if [ $storage = spi ]; then
		echo "use_spi=y" >> $output
		echo "use_nand=n" >> $output
		echo "use_emmc=n" >> $output
	elif [ $storage = nand ]; then
		echo "use_spi=n" >> $output
		echo "use_nand=y" >> $output
		echo "use_emmc=n" >> $output
	elif [ $storage = emmc ]; then
		echo "use_spi=n" >> $output
		echo "use_nand=n" >> $output
		echo "use_emmc=y" >> $output
	else
		echo "Please define strage type in feeds.conf"
		exit 1
	fi	
	echo "total_size=$storage_size" >> $output
	echo "align_size=$storage_align" >> $output
	echo "update_1stfw=$update_1stfw" >> $output
	echo "update_2ndfw=$update_2ndfw" >> $output
	if [ $update_1stfw = y ]; then
		echo "seqnum_1stfw=$seqnum_1stfw" >> $output
	fi
	if [ $update_2ndfw = y ]; then
		echo "seqnum_2ndfw=$seqnum_2ndfw" >> $output
	fi

	echo "burn_fwtbl=$burn_fwtbl" >> $output
	echo "burn_kernel=$burn_kernel" >> $output
	echo "burn_kerneldtb=$burn_kerneldtb" >> $output
	echo "burn_rescuedtb=$burn_rescuedtb" >> $output
	echo "burn_rescue_rootfs=$burn_rescue_rootfs" >> $output
	if [ $bluecore_exist = 1 ]; then
		echo "burn_bluecore=$burn_bluecore" >> $output
	fi
	if [ $bootlogo_exist = 1 ]; then
		echo "burn_bootlogo=$burn_bootlogo" >> $output
	fi
	
	echo "burn_bootpart=$burn_bootpart" >> $output
	echo "swap_part=$swap_part" >> $output
	echo "swap_part_size_MB=$swap_part_size_MB" >> $output

	if [ $bootargs_exist = 1 ]; then
		printf "factory = _%s\n" $factory_file >> $output
		echo "factory_start_addr=$factory_addr" >> $output
		echo "factory_zone=$factory_zone" >> $output
		echo "burn_factory=$burn_factory" >> $output
	fi

	if [ $update_1stfw = y ]; then
		#insert a '_' before the filename to makd a workaround for sscanf always read a \0 in the first byte of filename
		printf "1stfw = kernelDT _%s %s 2100000 %x %x %x\n" $kerneldtb_file $kernel_dtb_compress $kerneldtb_1stfw_addr $kerneldtb_filesize $kerneldtb_zone >> $output
		printf "1stfw = rescueDT _%s %s 2100000 %x %x %x\n" $rescuedtb_file $rescue_dtb_compress $rescuedtb_1stfw_addr $rescuedtb_filesize $rescuedtb_zone >> $output
		printf "1stfw = rescueRootFS _%s %s 2200000 %x %x %x\n" $rescuefs_file $rescue_rootfs_compress $rescuefs_1stfw_addr $rescuefs_filesize $rescuefs_zone >> $output
		if [ $bluecore_exist = 1 ]; then
			printf "1stfw = audioKernel _%s %s %x %x %x %x\n"  $bluecore_file $bluecore_compress $bluecore_mem_addr $bluecore_1stfw_addr $bluecore_filesize $bluecore_zone >> $output
		fi
		if [ $bootlogo_exist = 1 ]; then
			printf "1stfw = bootLogo _%s %s 30000000 %x %x %x\n" $bootlogo_file $bootlogo_compress $bootlogo_1stfw_addr $bootlogo_filesize $bootlogo_zone >> $output
		fi
		printf "1stfw = linuxKernel _%s %s 3000000 %x %x %x\n"  $kernel_file $kernel_compress $kernel_1stfw_addr $kernel_filesize $kernel_zone >> $output
	fi

	if [ $update_2ndfw = y ]; then
		#insert a '_' before the filename to makd a workaround for sscanf always read a \0 in the first byte of filename
		printf "2ndfw = kernelDT _%s %s 2100000 %x %x %x\n" $kerneldtb_file $kernel_dtb_compress $kerneldtb_2ndfw_addr $kerneldtb_filesize $kerneldtb_zone >> $output
		printf "2ndfw = rescueDT _%s %s 2100000 %x %x %x\n" $rescuedtb_file $rescue_dtb_compress $rescuedtb_2ndfw_addr $rescuedtb_filesize $rescuedtb_zone >> $output
		printf "2ndfw = rescueRootFS _%s %s 2200000 %x %x %x\n" $rescuefs_file $rescue_rootfs_compress $rescuefs_2ndfw_addr $rescuefs_filesize $rescuefs_zone >> $output
		if [ $bluecore_exist = 1 ]; then
			printf "2ndfw = audioKernel _%s %s %x %x %x %x\n"  $bluecore_file $bluecore_compress $bluecore_mem_addr $bluecore_2ndfw_addr $bluecore_filesize $bluecore_zone >> $output
		fi
		if [ $bootlogo_exist = 1 ]; then
			printf "2ndfw = bootLogo _%s %s 30000000 %x %x %x\n" $bootlogo_file $bootlogo_compress $bootlogo_2ndfw_addr $bootlogo_filesize $bootlogo_zone >> $output
		fi
		printf "2ndfw = linuxKernel _%s %s 3000000 %x %x %x\n"  $kernel_file $kernel_compress $kernel_2ndfw_addr $kernel_filesize $kernel_zone >> $output
	fi

	if [ $bootpart_exist = 1 ]; then
		printf "bootpart = %s _%s %x %x %x\n" $bootpart_type $bootpart_file $bootpart_addr $bootpart_filesize $bootpart_zone >> $output
	fi
}

function build_install_img() {
	workspace=$1/$project
	output=$workspace/../../install.img
	if [ ! -e Toolchain ]; then
		ln -s ../Toolchain
	fi
	if [ ! -e Toolchain/gcc-arm-8.2-2018.11-x86_64-arm-linux-gnueabihf ]; then 
		pushd Toolchain
		./arm-linux-gnueabihf.sh		
		popd
	fi
	make -C $TOPDIR/arm_bin/installer clean > /dev/null
	CROSS_COMPILER=$TOPDIR/Toolchain/gcc-arm-8.2-2018.11-x86_64-arm-linux-gnueabihf/bin/arm-linux-gnueabihf- make -C $TOPDIR/arm_bin/installer > /dev/null
	cp $TOPDIR/arm_bin/flash_erase $workspace
	cp $TOPDIR/arm_bin/installer/installer $workspace
	pushd $workspace
	tar -cvf $output * > /dev/null
	popd
}


if [ "$#" -ne 2 ]; then
	msg "Usage: build_image.sh feed_dir_path [rtd16xx_spi|rtd16xx_emmc|rtd129x_emmc]"
	exit 1	
fi

if [ ! -d $1 ]; then
	msg "feed_dir_path %1 not found"
	exit 1
fi

feed_dir=$1
chip=$2

if [ ! -e $feed_dir/feeds.conf ]; then
	msg "$feed_dir/feeds.conf not found"
	exit 1
fi

case "$chip" in
	rtd16xx_spi)
		storage_start_addr=$((0x130000))
		factory_addr=$((0x110000))
		bluecore_mem_addr=$((0xf900000))
		;;

	rtd16xx_emmc)
		storage_start_addr=$((0xa31000))
		factory_addr=$((0x621000))
		bluecore_mem_addr=$((0xf900000))
		;;
	
	rtd129x_spi)
		storage_start_addr=$((0x130000))
		factory_addr=$((0x110000))
		bluecore_mem_addr=$((0x1b00000))
		;;
	
	rtd129x_emmc)
		storage_start_addr=$((0x630000))
		factory_addr=$((0x220000))
		bluecore_mem_addr=$((0x1b00000))
		;;
	
	*)
		msg "chip should be [rtd16xx_spi|rtd16xx_emmc|rtd129x_spi|rtd129x_emmc]"
		exit 1
esac

. $feed_dir/feeds.conf

check_must_file $feed_dir/$kernel_file
check_must_file $feed_dir/$kerneldtb_file
check_must_file $feed_dir/$rescuedtb_file
check_must_file $feed_dir/$rescuefs_file
check_file $feed_dir/$bluecore_file; bluecore_exist=$?
check_file $feed_dir/$bootargs; bootargs_exist=$?
check_file $feed_dir/$bootlogo_file ; bootlogo_exist=$?
check_fsdir "$bootpart_dir"; bootpart_exist=$?
#check_fsdir "$feed_dir/$part2_dir"; part2_exist=$?

# rescue files
kernel_rsq=$kernel_file
dtb_rsq=$rescuedtb_file
rootfs_rsq=$rescuefs_file

WORKSPACE=$TOPDIR/workspace
img_dir=$WORKSPACE/$project
rm -rf $WORKSPACE; mkdir -p $img_dir

function check_zone_size() {
	filesize=$1
	zonesize=$2
	zonename=$3
	
	if [ $filesize -gt $zonesize ]; then
		msg ""$zonename"_zone size ($zonesize) < $zonename filesize $padding_size"
		exit 1
	fi

}

function prepare_kerneldtb() {
#kerneldtb
	align_check kerneldtb_zone $kerneldtb_zone
	dtc -I dtb  $feed_dir/$kerneldtb_file | dtc -p 1024 -O dtb > $WORKSPACE/$kerneldtb_file.enlarge
	check_dtc $WORKSPACE/$kerneldtb_file.enlarge
	filesize=`ls -la $WORKSPACE/$kerneldtb_file.enlarge | awk '{print $5}'`
	get_align_size $filesize; padding_size=$ret_get_align_size

	check_zone_size $padding_size $kerneldtb_zone kerneldtb

	padding $WORKSPACE/$kerneldtb_file.enlarge $img_dir/$kerneldtb_file.padding $padding_size; kerneldtb_filesize=$padding_size
	kerneldtb_file=$kerneldtb_file.padding
	kernel_dtb_compress=raw

	if [ $update_1stfw = y ]; then
		[ $storage_size -lt $((kerneldtb_zone+kerneldtb_1stfw_addr)) ] && msg "1st kernel size($((kerneldtb_zone+kerneldtb_1stfw_addr))) exceed max storage size($storage_size)"
	fi

	if [ $update_2ndfw = y ]; then
		[ $storage_size -lt $((kerneldtb_zone+kerneldtb_2ndfw_addr)) ] && msg "2nd kernel size($((kerneldtb_zone+kerneldtb_2ndfw_addr))) exceed max storage size($storage_size)"
	fi
}

function prepare_rescuedtb() {
#rescuedtb
	align_check rescuedtb_zone $rescuedtb_zone
	dtc -I dtb  $feed_dir/$rescuedtb_file | dtc -p 1024 -O dtb > $WORKSPACE/$rescuedtb_file.enlarge
	check_dtc $WORKSPACE/$rescuedtb_file.enlarge
	filesize=`ls -la $WORKSPACE/$rescuedtb_file.enlarge | awk '{print $5}'`
	get_align_size $filesize; padding_size=$ret_get_align_size

	check_zone_size $padding_size $rescuedtb_zone rescuedtb

	padding $WORKSPACE/$rescuedtb_file.enlarge $img_dir/$rescuedtb_file.padding $padding_size; rescuedtb_filesize=$padding_size
	rescuedtb_file=$rescuedtb_file.padding
	rescue_dtb_compress=raw
	if [ $update_1stfw = y ]; then
		[ $storage_size -lt $((rescuedtb_zone+rescuedtb_1stfw_addr)) ] && msg "1st kernel size($((rescuedtb_zone+rescuedtb_1stfw_addr))) exceed max storage size($storage_size)"
	fi

	if [ $update_2ndfw = y ]; then
		[ $storage_size -lt $((rescuedtb_zone+rescuedtb_2ndfw_addr)) ] && msg "2nd kernel size($((rescuedtb_zone+rescuedtb_2ndfw_addr))) exceed max storage size($storage_size)"
	fi
}

function prepare_rescuefs() {
	#rescuefs
	align_check rescuefs_zone $rescuefs_zone
	cp $feed_dir/$rescuefs_file $WORKSPACE/$rescuefs_file
	rescue_rootfs_compress=raw
	filesize=`ls -la $WORKSPACE/$rescuefs_file | awk '{print $5}'`
	get_align_size $filesize; padding_size=$ret_get_align_size

	check_zone_size $padding_size $rescuefs_zone rescuefs

	padding $WORKSPACE/$rescuefs_file $img_dir/$rescuefs_file.padding $padding_size; rescuefs_filesize=$padding_size
	rescuefs_file=$rescuefs_file.padding
	if [ $update_1stfw = y ]; then
		[ $storage_size -lt $((rescuefs_zone+rescuefs_1stfw_addr)) ] && msg "1st kernel size($((rescuefs_zone+rescuefs_1stfw_addr))) exceed max storage size($storage_size)"
	fi

	if [ $update_2ndfw = y ]; then
		[ $storage_size -lt $((rescuefs_zone+rescuefs_2ndfw_addr)) ] && msg "2nd kernel size($((rescuefs_zone+rescuefs_2ndfw_addr))) exceed max storage size($storage_size)"
	fi
}

function prepare_bluecore() {
	#bluecore
	if [ $bluecore_exist = 1 ]; then
		align_check bluecore_zone $bluecore_zone
		if [ $lzma = y ]; then
			$TOPDIR/x86_bin/lzma e $feed_dir/$bluecore_file $WORKSPACE/$bluecore_file.lzma > /dev/null; [ ! $? ] && msg "lzma $feed_dir/$bluecore_file fail $?" && exit 1
			bluecore_file=$bluecore_file.lzma
			bluecore_compress=lzma
		else
			cp $feed_dir/$bluecore_file $WORKSPACE/$bluecore_file
			bluecore_compress=raw
		fi
		filesize=`ls -la $WORKSPACE/$bluecore_file | awk '{print $5}'`
		get_align_size $filesize; padding_size=$ret_get_align_size

		check_zone_size $padding_size $bluecore_zone bluecore

		padding $WORKSPACE/$bluecore_file $img_dir/$bluecore_file.padding $padding_size; bluecore_filesize=$padding_size
		bluecore_file=$bluecore_file.padding
	fi

	if [ $update_1stfw = y ]; then
		[ $storage_size -lt $((bluecore_zone+bluecore_1stfw_addr)) ] && msg "1st kernel size($((bluecore_zone+bluecore_1stfw_addr))) exceed max storage size($storage_size)"
	fi

	if [ $update_2ndfw = y ]; then
		[ $storage_size -lt $((bluecore_zone+bluecore_2ndfw_addr)) ] && msg "2nd kernel size($((bluecore_zone+bluecore_2ndfw_addr))) exceed max storage size($storage_size)"
	fi
}

function prepare_kernel() {
	#kernel
	align_check kernel_zone $kernel_zone
	if [ $lzma = y ]; then
	 	$TOPDIR/x86_bin/lzma e $feed_dir/$kernel_file $WORKSPACE/$kernel_file.lzma > /dev/null; [ ! $? ] && msg "lzma $feed_dir/$kernel_file fail $?" && exit 1
		kernel_file=$kernel_file.lzma
		kernel_compress=lzma
	else
		cp $feed_dir/$kernel_file $WORKSPACE/$kernel_file
		kernel_compress=raw
	fi
	filesize=`ls -la $WORKSPACE/$kernel_file | awk '{print $5}'`
	get_align_size $filesize; padding_size=$ret_get_align_size

	check_zone_size $padding_size $kernel_zone kernel

	padding $WORKSPACE/$kernel_file $img_dir/$kernel_file.padding $padding_size; kernel_filesize=$padding_size
	kernel_file=$kernel_file.padding

	if [ $update_1stfw = y ]; then
		[ $storage_size -lt $((kernel_zone+kernel_1stfw_addr)) ] && msg "1st kernel size($((kernel_zone+kernel_1stfw_addr))) exceed max storage size($storage_size)"
	fi

	if [ $update_2ndfw = y ]; then
		[ $storage_size -lt $((kernel_zone+kernel_2ndfw_addr)) ] && msg "2nd kernel size($((kernel_zone+kernel_2ndfw_addr))) exceed max storage size($storage_size)"
	fi
}

function perpare_bootlogo() {
	if [ $bootlogo_exist = 1 ]; then
		align_check bootlogo_zone $bootlogo_zone
		if [ $lzma = y ]; then
			$TOPDIR/x86_bin/lzma e $feed_dir/$bootlogo_file $WORKSPACE/$bootlogo_file.lzma > /dev/null; [ ! $? ] && msg "lzma $feed_dir/$bootlogo_file fail $?" && exit 1
			bootlogo_file=$bootlogo_file.lzma
			bootlogo_compress=lzma
		else
			cp $feed_dir/$bootlogo_file $WORKSPACE/$bootlogo_file
			bootlogo_compress=raw
		fi
		filesize=$(ls -la $WORKSPACE/$bootlogo_file | awk '{print $5}')
		get_align_size $filesize; padding_size=$ret_get_align_size

		check_zone_size $padding_size $bootlogo_zone bootlogo_zone

		padding $WORKSPACE/$bootlogo_file $img_dir/$bootlogo_file.padding $padding_size; bootlogo_filesize=$padding_size
		bootlogo_file=$bootlogo_file.padding
		if [ $update_1stfw = y ]; then
			[ $storage_size -lt $((bootlogo_zone+bootlogo_1stfw_addr)) ] && msg "1st kernel size($((bootlogo_zone+bootlogo_1stfw_addr))) exceed max storage size($storage_size)"
		fi

		if [ $update_2ndfw = y ]; then
			[ $storage_size -lt $((bootlogo_zone+bootlogo_2ndfw_addr)) ] && msg "2nd kernel size($((bootlogo_zone+bootlogo_2ndfw_addr))) exceed max storage size($storage_size)"
		fi
	fi
}

function prepare_bootpart() {
	#partition 1
	if [ $bootpart_exist = 1 ]; then
		align_check bootpart_zone $bootpart_zone
		bootpart_file=bootpart.img
		#sudo rm -rf $WORKSPACE/$bootpart_dir
		#sudo cp -a $feed_dir/$bootpart_dir $WORKSPACE/$bootpart_dir 
		#sudo chown root:root $WORKSPACE/$bootpart_dir
		if [ $bootpart_type = squashfs ]; then
			$TOPDIR/x86_bin/mksquashfs_nas $feed_dir/$bootpart_dir $WORKSPACE/$bootpart_file -comp xz 
			[ ! $? = 0 ] && msg "$TOPDIR/x86_bin/mksquashfs_nas $feed_dir/$bootpart_dir $WORKSPACE/$bootpart_file -comp xz Fail"
		fi
		#if [ $bootpart_type = ubifs ]; then
			#todo
		#fi

		#if [ $bootpart_type = ext4 ]; then
			#todo
		#fi

		filesize=`ls -la $WORKSPACE/$bootpart_file | awk '{print $5}'`
		get_align_size $filesize; padding_size=$ret_get_align_size

		check_zone_size $padding_size $bootpart_zone $bootpart_type
		
		padding $WORKSPACE/$bootpart_file $img_dir/$bootpart_file.padding $padding_size; bootpart_filesize=$alignedsize
		bootpart_file=$bootpart_file.padding
	else
		if [ $storage = emmc ]; then
			msg "Must add rootfs for the boot partition in eMMc"
			exit 1
		fi
	fi
}

function check_layout() {
	workspace=$1/$project
	# compile layout tool
	make -C $TOPDIR/x86_bin/storage_layout clean > /dev/null
	make -C $TOPDIR/x86_bin/storage_layout > /dev/null
	if [ $? -ne 0 ]; then
		msg "layout tool error"
		exit 1
	fi

	# run layout tool
	debug=0
	$TOPDIR/x86_bin/storage_layout/layout-checker $workspace/config.txt $debug
	if [ $? -ne 0 ]; then
		msg "layout check error!"
		exit 1
	fi
}

function build_rescue_file() {
	workspace=$1/rescue
	rm -rf $workspace; mkdir -p $workspace
	# copy kernel & rescuedtb and pad rescuefs to 1 MB
	cp feed/$kernel_rsq $workspace/$storage.uImage
	cp feed/$dtb_rsq $workspace/rescue.$storage.dtb
	dd if=feed/$rootfs_rsq of=$workspace/rescue.root.$storage.cpio.gz_pad.img bs=1048576 count=1 conv=sync 2>/dev/null
}

function prepare_factory() {
	if [ $bootargs_exist = 1 ]; then
		if [ $storage = emmc ]; then
			# $((128<<10)) defined in bootcode "include/configs/rtd161x_common.h"
			envtxt_size=$((128 << 10))
			factory_zone=$((0x400000))
		fi
		if [ $storage = spi ]; then
			envtxt_size=$((8192))
			factory_zone=$((0x20000))
		fi
		envtxt_header_size=4
		envtxt_content_size=$(($envtxt_size - $envtxt_header_size))

		align_check factory_zone $factory_zone
		factory_file="factory.tar"
		check_must_file $feed_dir/$factory_file

		dd if=/dev/zero bs=1 count=$envtxt_content_size of="env_content.bin" &> /dev/null
		awk -v storage=$storage -f "make_envcontent.awk" $feed_dir/$bootargs > "tmp.bin" 
		dd if="tmp.bin" of="env_content.bin" conv=notrunc &> /dev/null

		# write the crc32 data to header in big-endian format
                check_crc
		crc="$( crc32 "env_content.bin" )"
		crc="0x"$crc
		mask=$((0xff))
		for i in $(seq 0 8 24); do
			write_byte=$(( $crc & $mask ))
			write_byte=$(( $write_byte >> $i ))
			printf "\\x$(printf "%x" $write_byte)" >> "env_header.bin"
			mask=$(( $mask << 8 ))
		done

		dd if=/dev/zero of="env.txt" bs=1 count=$envtxt_size &> /dev/null
		dd if="env_header.bin" of="env.txt" conv=notrunc bs=1 &> /dev/null
		dd if="env_content.bin" of="env.txt" oflag=seek_bytes conv=notrunc \
			bs=1 seek=$envtxt_header_size &> /dev/null

		cp $feed_dir/$factory_file $factory_file
		tar -f $factory_file --delete "env.txt"
		tar -f $factory_file --append "env.txt"
		cp $factory_file $WORKSPACE/$factory_file

		clean_filelist=(env_content.bin env_header.bin env.txt tmp.bin $factory_file)
		for clean_file in ${clean_filelist[@]}; do
			clean $clean_file
		done

		filesize=$(ls -la $WORKSPACE/$factory_file | awk '{print $5}')
		get_align_size $filesize; padding_size=$ret_get_align_size

		check_zone_size $padding_size $factory_zone "factory"
		
		padding $WORKSPACE/$factory_file $img_dir/$factory_file.padding $padding_size; factory_filesize=$alignedsize
		factory_file=$factory_file.padding
	fi
}

prepare_kerneldtb
prepare_rescuedtb
prepare_rescuefs
prepare_kernel
prepare_bluecore
perpare_bootlogo
prepare_bootpart
prepare_factory

build_config_txt  $WORKSPACE
check_layout $WORKSPACE

build_rescue_file $WORKSPACE
build_install_img $WORKSPACE


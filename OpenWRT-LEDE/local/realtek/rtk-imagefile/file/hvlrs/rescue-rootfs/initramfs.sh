#!/bin/bash
#set -x

function get_align_size() {
	size=$1
	if [ $((size&$((storage_align-1)))) != 0 ]; then 
		ret_get_align_size=$(($((size+storage_align))-$((size&$((storage_align-1))))))
	else
		ret_get_align_size=$size
	fi
}
function align_and_padding() {
	source=$1
	target=$2
	filesize=`ls -la $source | awk '{print $5}'`
	get_align_size $filesize; alignedsize=$ret_get_align_size
	dd if=$source of=$target bs=$alignedsize count=1 conv=sync 2> /dev/null	 
}
[ $# = 0 ] && echo -e "usage ./initramfs.sh [decompress|compress] \n input:rescue_rootfs.cpio.gz \n output:rescue_updated.cpio.gz" && exit 1
if [ $1 = 'decompress' ]; then
	echo "need root permission"
	sudo rm -rf rootfs;mkdir -p rootfs
	pushd rootfs > /dev/null
	gzip -n -9 -f -qdc ../rescue_rootfs.cpio.gz | cpio --quiet -idv > /dev/null
	popd > /dev/null
	exit 0
fi

if [ $1 = 'compress' ]; then
	pushd rootfs > /dev/null
	echo "need root permission"
	sudo chown root:root -R *
	find | cpio -H newc -o | gzip > ../rescue_rootfs.cpio.gz
#	find | cpio -H newc -o | gzip > ../rescue_updated.cpio.gz
	popd > /dev/null
#	dd if=rescue_updated.cpio.gz of=rescue_updated.cpio.gz_pad.img bs=$((1024*1024)) count=1 conv=sync 2> /dev/null
#	sudo rm rescue_updated.cpio.gz
	exit 0
fi




#!/bin/bash
#set -x
CPU=`grep -c ^processor /proc/cpuinfo`
[ $# -ne 1 ] && echo -e "Usage:\n ./build.sh [DEFCONFIG]\nAvailable DEFCONFIG:\n\trtd129x_HDMI_defconfig\n\trtd129x_MM_defconfig\n\trtd129x_opencl_defconfig\n" && exit 1
[ ! -e Toolchain ] && ln -s ../Toolchain
pushd Toolchain
	./aarch64-linux-gnu.sh
	TOOLCHAIN=`pwd`/gcc-arm-8.2-2018.11-x86_64-aarch64-linux-gnu/bin
popd

pushd kernel
ARCH=arm64  make $1
ARCH=arm64 CROSS_COMPILE=$TOOLCHAIN/aarch64-linux-gnu- make -j$CPU
popd

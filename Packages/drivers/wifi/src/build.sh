#!/bin/bash
#set -x
TOP=`pwd`
CC=$TOP/../../../../OpenWRT-LEDE/staging_dir/toolchain-aarch64_cortex-a53_gcc-8.2.0_glibc/bin/aarch64-openwrt-linux-gnu-
KS=$TOP/../../../../OpenWRT-LEDE/build_dir/target-aarch64_cortex-a53_glibc/linux-realtek_rtd129x/linux-external/
CPUNUM=`grep -c ^processor /proc/cpuinfo`

# MUST Kernel Configs
# CONFIG_CFG80211=y
# CONFIG_CFG80211_DEFAULT_PS=y
# CONFIG_CFG80211_CRDA_SUPPORT=y
function build_src {
  #Define STAGING_DIR if using openwrt toolchain
  STAGING_DIR=$TOP/../../../../../OpenWRT-LEDE/staging_dir \
  CROSS=$CC \
  LINUX_KERNEL_PATH=$KS \
  make  -j$CPUNUM;
}

function check_chip_validity {
  #Check the validity of IC.
  if [ -d "$1" ]; then
    echo -e "Build $1 Driver \nwith Kernel Path:\n $KS\nby Toolchain:\n$CC\n"
    chip_is_vaild=1
    break
  else
    chip_is_vaild=0
  fi

  if [ "$chip_is_vaild" == 0 ]; then
    echo -e "\nThe $1 won't support or sharing driver."
    echo -e "Please input complete name of IC,ex:rtl8822ce\nor contact vendor for more assistance.\n"
    exit 0
  fi

  return "$chip_is_vaild"
}

if [ $# -eq 1 ]; then
  check_chip_validity $1
  if [ "$chip_is_vaild" == 1 ]; then
    cd $1
    build_src
    exit 0
  fi
fi

if [ $# -eq 2 ] && [ "$2" == "clean" ]; then
  check_chip_validity $1
  echo -e "Clean $1 source tree"
  cd $1
  make clean
  exit 0
fi

echo -e "Usage: \n\t./build.sh [IC]        =>Build WiFi Driver\n\t./build.sh [IC][clean] =>Clean Driver Source\n"
echo -e "MUST Turn ON Following Kernel Config First\nCONFIG_CFG80211=y\nCONFIG_CFG80211_DEFAULT_PS=y\nCONFIG_CFG80211_CRDA_SUPPORT=y\n"





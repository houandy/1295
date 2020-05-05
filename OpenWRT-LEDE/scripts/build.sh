#!/bin/sh

# ex: ./script/build.sh 1619_kernel49_nas build 1.0.0.0(PKG version) 1.0.0.0(FW version)


workdir=$(pwd)
[ -z $1 ] && modelname="1619_kernel49_nas" || modelname=$1
[ -z $2 ] && build_mode="build" || build_mode=$2
[ -z $3 ] && VERSION_CODE="0.0.0" || VERSION_CODE=$3
[ -z $4 ] && VERSION_NUMBER="0.0.0" || VERSION_NUMBER=$4
UBOOT_VERSION="1.0.0"
branch=master
now="$(date '+%Y%m%d')"
FOLDER=${workdir}
CONFIGFOLDER=${FOLDER}/configs/
binfilefolder=bin/targets/realtek/rtd16xx-glibc/
dailyimagefolder=${FOLDER}/../../dailyimage
DEST_FILE=install.img
DEST_ROOTFS_FILE=root.tar.bz2
releaseimagepath="/var/www/html/release/image/${modelname}/"
releaseimagefolder="${releaseimagepath}/FW-${modelname}-${VERSION_NUMBER}-${now}/"
upgradefwfile=${modelname}-upgradeimage-${now}.zip
upgradefwbin=${modelname}-upgradeimage-${now}.bin
MD5SUM=fwmd5
modelnamefile=modelname
soc=rtd16xx

[ -f "${FOLDER}/Makefile" -a -f "${FOLDER}/rules.mk" ] || {
  echo "run script path error!"
  exit
}

cleanbinfile()
{
	cd $1 && rm -rf ${binfilefolder} && cd ..
}

runclean()
{
  cd $1
  rm -rf tmp
  make target/linux/clean
  make package/clean
  feed_clean $1
  git reset --hard HEAD && git clean -f -d $1
}

setconfig()
{
 cd $1
 [ -f .config.bak ] && rm -f .config.bak
 [ -f .config ] && cp .config .config.bak
 rm -f .config
 checkmodel
 [ -f ${CONFIGFOLDER}/${modelname}.config ] && cp ${CONFIGFOLDER}/${modelname}.config .config
}

feed_clean()
{
  cd $1
  make package/symlinks-clean
  rm -rf feeds
}

feed_install()
{
  cd $1
  if [ -d $1/feeds ]; then
    [ -d "$1/package/feeds/" ] || make package/symlinks-install
  else
    make package/symlinks
  fi
}

initcode()
{
	[ ${branch} != "master" ] && {
		echo "change branch"
		cd $1
		git checkout $branch || exit
		cd ..
	}
	[ -d $1 ] && cd $1 && setconfig "$1"
	[ -d $1/package/feeds ] || feed_install "$1"
	if [ -f target/linux/realtek/image/rtk-imagefile/feed/feeds.conf ];then
		make package/feeds/realtek/rtk-imagefile/clean
	fi
	sed -i "2s/version=1.0.0/version=${VERSION_NUMBER}/" ../Image-Builder/feed/feeds.conf.emmc
	sed -i "2s/version=1.0.0/version=${VERSION_NUMBER}/" ../Image-Builder/feed/feeds.conf.spi
}

buildimage()
{
	initcode $1
	make defconfig
	make VERSION_PRODUCT=${modelname} VERSION_CODE=${VERSION_CODE} VERSION_NUMBER=${VERSION_NUMBER} BUILDTIME=${now}
	git checkout ../Kernel-Release/kernel/
	git checkout ../Image-Builder/feed/feeds.conf.*
}

checkmodel()
{
	[ ! -f "${CONFIGFOLDER}/${modelname}.config" ] && {
		echo "please check model name!"
		exit
	} || {
		soc=$(cat ${CONFIGFOLDER}/${modelname}.config | grep CONFIG_TARGET_realtek_ | grep "=y" | cut -d "=" -f 1 | cut -d "_" -f 4 | head -1)
		libc=$(cat ${CONFIGFOLDER}/${modelname}.config | grep "CONFIG_LIBC=" | cut -d "=" -f 2 | sed 's/"//g')
		binfilefolder="bin/targets/realtek/${soc}-${libc}/" 
	}
}

packimage()
{
	echo "$1/${binfilefolder}/${DEST_FILE}"
	cd $1/${binfilefolder}/ && mkdir -p packimagetmp
	cp -f $1/${binfilefolder}/${DEST_FILE} $1/${binfilefolder}/packimagetmp/
	cd $1/${binfilefolder}/packimagetmp/
	md5sum  * > ${MD5SUM}
#  setpkginfo ${PKG_INFO_FILE}

#  tar zcvf ${upgradefwfile} *
	zip --password qsidesa${modelname} ${upgradefwfile} *
#  zip ${upgradefwfile} *
	mv ${upgradefwfile} ../${upgradefwbin}
#  md5sum ${upgradefwfile} | cut -d ' ' -f1 > ../${upgradefwbin} && cat ${upgradefwfile} >> ../${upgradefwbin}
	cd .. && rm -rf packimagetmp/
	cd $1
}

cpimage()
{
	[ -d ${dailyimagefolder}/ ] || mkdir -p ${dailyimagefolder}/
	[ -d ${dailyimagefolder}/${modelname}/ ] || mkdir -p ${dailyimagefolder}/${modelname}/
	[ -d $2 ] || mkdir -p $2
	cp $1/${binfilefolder}/${DEST_FILE} $2/
	cp -Rf $1/${binfilefolder}/r* $2/
#	cp $1/${binfilefolder}/${upgradefwbin} $2/
# md5sum $2/* > $2/${MD5SUM}
}

if [ ! ${build_mode} = "all" ]; then
	if [ ${build_mode} = "clean" ]; then
		echo "clean"
		cleanbinfile
		runclean
	fi
	if [ ${build_mode} = "build" ]; then
		echo "buildimage"
		checkmodel
		cleanbinfile ${FOLDER}
		buildimage ${FOLDER}
#		[ -z $(cat .config | grep "CONFIG_RTK_MTD_NVR=y") ] && {
#			echo "packimage"
#			packimage ${FOLDER}
#		} || {
#			upgradefwbin="openwrt-${soc}-${modelname}-rtk-spi-8M-initrd-sysupgrade.bin"
#		}
		echo "cpimage"
		cpimage ${FOLDER} ${dailyimagefolder}/${modelname}/${now}
	fi
	if [ ${build_mode} = "packimage" ]; then
		echo "packimage"
		checkmodel
		[ -z $(cat ${CONFIGFOLDER}/${modelname}.config | grep "CONFIG_RTK_MTD_NVR=y") ] && {
			packimage ${FOLDER}
		}
	fi
	if [ ${build_mode} = "cpimage" ]; then
		echo "cpimage"
		checkmodel
		cpimage ${FOLDER} ${dailyimagefolder}/${modelname}/${now}
	fi
	if [ ${build_mode} = "release" ]; then
		echo "release"
		checkmodel
		[ -d ${releaseimagepath} ] || mkdir -p ${releaseimagepath}
		[ -d ${releaseimagefolder} ] || mkdir -p ${releaseimagefolder}
		[ -d ${releaseimagefolder} ] && {
		cp ${FOLDER}/${binfilefolder}/${DEST_FILE} ${releaseimagefolder}
		[ -f ${FOLDER}/${binfilefolder}/${DEST_ROOTFS_FILE} ] && cp ${FOLDER}/${binfilefolder}/${DEST_ROOTFS_FILE} ${releaseimagefolder}
		cp ${FOLDER}/${binfilefolder}/rescue/* ${releaseimagefolder}
		}
	fi
fi


#!/bin/sh
#set -x

hdd_str="sata"
#hdd_str="sd"
raid_name=QSI_NAS


rm_partition() {
	parted -s $1 rm $2
}

parted_hdd() {
	parted -s $1 mklabel gpt
	parted -s $1 "mkpart RAID ext4 0 -1"
	echo "=====finished to format hdd===="
}


default_md=$(uci get mdadm.@array[0].device)
[ -z "${default_md}" ] && umount /dev/md0 || umount $(uci get mdadm.@array[0].device)
/etc/init.d/mdadm stop


#umount all partition
for dev in $(mount | grep ${hdd_str} | cut -d " " -f 1);do
	umount ${dev}
done
sync


#check HDD
for dev in $(lsblk | grep ^${hdd_str} | cut -d " " -f1);do
	#rm all partition
	for list in $(lsblk | grep ${dev} | grep "-" | awk '{print $1}' | tail -c 2);do
		rm_partition "/dev/${dev}" $list
		sync
	done
	#create partition
	parted_hdd "/dev/${dev}"
	sync
	[ -n "${hdd_list}" ] && hdd_list=$(echo $hdd_list /dev/$dev) || hdd_list=$(echo /dev/$dev)
done
sync

echo "hdd_list=$hdd_list"


#setup mdadm for uci
uci delete mdadm.@array[0]
uci add mdadm array
uci set mdadm.@array[0].device=/dev/md0
uci set mdadm.@array[0].level=0
uci set mdadm.@array[0].member=2
uci set mdadm.@array[0].devices="${hdd_list}"
uci commit


#create RAID
mdadm -C $(uci get mdadm.@array[0].device) -l $(uci get mdadm.@array[0].level) -n $(uci get mdadm.@array[0].member) $(uci get mdadm.@array[0].devices) --run --assume-clean


sleep 1
[ -z "${default_md}" ] && umount /dev/md0 || umount $(uci get mdadm.@array[0].device)


#mkfs partition
mkfs.ext4 $(uci get mdadm.@array[0].device) -L $raid_name -F


#mount /dev/md0
block mount


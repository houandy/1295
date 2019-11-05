#!/bin/sh

flashimage="FLASH_$(cat /etc/modelname).bin"
partitiontable=$(cat /proc/mtd | grep mtd | cut -d ":" -f 1)

/root/utils/factory set fwversion $(cat /etc/SOC_FW_version) > /dev/null 2>&1
/root/utils/factory set $(cat /etc/modelname)version $(cat /etc/PKG_FW_version) > /dev/null 2>&1
/root/utils/factory save > /dev/null 2>&1

rm -f /tmp/${flashimage}
for partition in ${partitiontable}
do
	cat /dev/${partition} >> /tmp/${flashimage}
done

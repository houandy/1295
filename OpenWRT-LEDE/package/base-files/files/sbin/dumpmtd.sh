#!/bin/sh

flashimage="FLASH_$(cat /etc/modelname).bin"
partitiontable=$(cat /proc/mtd | grep mtd | cut -d ":" -f 1)

for partition in ${partitiontable}
do
	cat /dev/${partition} >> /tmp/${flashimage}
done

#!/bin/bash

hdd_temp_file="/tmp/hddthermal"
get_hdd_temp_count_file="/tmp/hddthermalcount"
hdd_temp=40
soc_temp=80
hdd_limit_temp=48
hddthermal=
socthermal=
fast_speed=70
high_speed=50
low_speed=30
count=1
min=

get_hdd_temp()
{
	hddthermal=$(thermal hdd | grep Temp | awk -F " " '{print $2}')
	echo $hddthermal > ${hdd_temp_file}
	echo 1 > ${get_hdd_temp_count_file}
}

fan()
{
	socthermal=$(thermal soc | grep Temp | awk -F "=" '{print $2}' | awk -F "." '{print $1}')
	if [ -f $hdd_temp_file ] ; then
		hddthermal=$(cat ${hdd_temp_file})
		c1=$(($hdd_temp-$hddthermal))
		c2=$(($hdd_limit_temp-$hddthermal))
		cc1=${c1#-}
		cc2=${c2#-}
		[ $cc1 -le $cc2 ] && min=$cc1 || min=$cc2
		cc=$(($min*2))
		[ $1 -ge $cc ] && get_hdd_temp
	else
		get_hdd_temp
	fi

	dutyrate=$(cat /sys/devices/platform/980070d0.pwm/dutyRate0  | cut -d "%" -f1)
	[ $hddthermal -gt "20" ] || return 0
	[ $socthermal -gt "40" ] || return 0

	if [ "$hddthermal" -ge "$hdd_temp" ] || [ "$socthermal" -ge "$soc_temp" ] ; then
		if [ "$dutyrate" -lt "$high_speed" ]; then
			echo $high_speed > /sys/devices/platform/980070d0.pwm/dutyRate0
		else
			if [ "$hddthermal" -ge "$hdd_limit_temp" ]; then
				if [ "$dutyrate" -lt "$fast_speed" ]; then
					echo $fast_speed > /sys/devices/platform/980070d0.pwm/dutyRate0
				fi
			else
				if [ "$dutyrate" -eq "$fast_speed" ]; then
					echo $high_speed > /sys/devices/platform/980070d0.pwm/dutyRate0
				fi
			fi
		fi
	else
		if [ "$dutyrate" -gt "$low_speed" ]; then
			echo $low_speed > /sys/devices/platform/980070d0.pwm/dutyRate0
		fi
	fi
}

if [ -f ${get_hdd_temp_count_file} ]; then
	count=$(cat $get_hdd_temp_count_file)
	echo $((count+1)) > $get_hdd_temp_count_file
else
	echo 1 > ${get_hdd_temp_count_file}
fi

fan $count


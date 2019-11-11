#!/bin/bash

temperature=40
hddthermal=
high_speed=50
low_speed=35

fan()
{
	hddthermal=$(thermal hdd | grep Temp | awk -F " " '{print $2}')
	dutyrate=$(cat /sys/devices/platform/980070d0.pwm/dutyRate0  | cut -d "%" -f1)
	[ $hddthermal -gt "20" ] || return

	if [ "$hddthermal" -ge "$temperature" ]; then
		if [ "$dutyrate" -lt "$high_speed" ]; then
			[ -f /tmp/fan_high ] &&	{
				echo $high_speed > /sys/devices/platform/980070d0.pwm/dutyRate0
			} || {
				touch /tmp/fan_high
				return
			}
		fi
	else
		if [ "$dutyrate" -gt "$low_speed" ]; then
			[ -f /tmp/fan_low ] && {
				echo $low_speed > /sys/devices/platform/980070d0.pwm/dutyRate0
			} || {
				touch /tmp/fan_low
				return
			}
		fi
	fi
	rm /tmp/fan_* > /dev/null 2>&1
}

fan
exit 0


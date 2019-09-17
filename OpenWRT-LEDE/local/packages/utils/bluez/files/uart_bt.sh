#!/bin/sh

look_for_hci() {
	
	i=0
	while [ $i -le 10 ]
	do
		sleep 1
		[ -n "`/usr/bin/hciconfig hci0`" ] && return 0
		i=$((i+1))
	done

	return 1
}

# enable rtk_hciattach
look_for_hci

if [ $? -ne 0 ]; then
	return 1;
fi

[ -z "`pgrep /usr/bin/bluetoothd`" ] && {
	/usr/bin/bluetoothd -n -C -d --noplugin=sap &
}

#[ "$1" != "" ] && {
#	/usr/bin/bluez/test-adapter alias "$1"
#}

hciconfig hci0 reset && hciconfig hci0 up

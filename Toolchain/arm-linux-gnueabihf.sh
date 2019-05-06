#!/bin/bash
#arm gcc
if [ ! -e gcc-arm-8.2-2018.11-x86_64-arm-linux-gnueabihf.tar.xz ]; then
wget --no-check-certificate https://developer.arm.com/-/media/Files/downloads/gnu-a/8.2-2018.11/gcc-arm-8.2-2018.11-x86_64-arm-linux-gnueabihf.tar.xz
fi
if [ ! -e gcc-arm-8.2-2018.11-x86_64-arm-linux-gnueabihf ]; then 
tar xvf ./gcc-arm-8.2-2018.11-x86_64-arm-linux-gnueabihf.tar.xz
fi
echo "Done!"

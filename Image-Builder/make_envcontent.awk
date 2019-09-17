#!/usr/bin/awk -f


BEGIN{
    # a rarely used character to prevent seperate a line
    FS="\x07"
    
    arg["audio_loadaddr"]=audio_memaddr
    arg["baudrate"]="115200"
    arg["blue_logo_loadaddr"]="0x30000000"
    arg["bootcmd"]="bootr"
    arg["bootdelay"]="0"
    arg["dtbo_loadaddr"]="0x26400000"
    arg["eth_drv_para"]="phy,bypass,noacp,swing0"
    arg["fdt_high"]="0xffffffffffffffff"
    arg["fdt_loadaddr"]="0x02100000"
    arg["kernel_loadaddr"]="0x03000000"
    arg["mtd_part"]="mtdparts=rtk_nand:"
    arg["rescue_audio"]="bluecore.audio"
    if (storage=="spi")
    {
        arg["rescue_dtb"]="rescue.spi.dtb"
        arg["rescue_rootfs"]="rescue.root.spi.cpio.gz_pad.img"
        arg["rescue_vmlinux"]="spi.uImage"
    }
    if (storage=="emmc")
    {
        arg["rescue_dtb"]="rescue.emmc.dtb"
        arg["rescue_rootfs"]="rescue.root.emmc.cpio.gz_pad.img"
        arg["rescue_vmlinux"]="emmc.uImage"
    }
    if (storage=="nand")
    {
        arg["rescue_dtb"]="rescue.nand.dtb"
        arg["rescue_rootfs"]="rescue.root.nand.cpio.gz_pad.img"
        arg["rescue_vmlinux"]="nand.uImage"
    }
    arg["rescue_rootfs_loadaddr"]="0x02200000"
    if ((chip=="rtd129x_spi"))
    {
        arg["rootfs_loadaddr"]="0x04000000"
    }
    else
    {
        arg["rootfs_loadaddr"]="0x02200000"
    }
    arg["serverip"]="192.168.100.2"
}
{
    # index() find the string "=" for the first occurrence in $1, and return the position in characters
    pos = index($1, "=")
    
    arg_val = substr($1, pos+1)
    # we substract 1 from pos so that the string "arg_idx" won't trail with "="
    arg_idx = substr($1, 0, pos-1)
    # array "arg" has name of env variable, which is a string, as index
    arg[arg_idx]=arg_val
}
END{
    # the function "asorti" sort the string index in dictionary order for us, and return size of array "arg"
    # you can access sorted string index by passing consecutive integers into the new array "sorted_idx"
    n = asorti(arg, sorted_idx)
    for (i = 1; i <= n; i++) {
        printf("%s=%s\0",sorted_idx[i],arg[sorted_idx[i]])
    }
}

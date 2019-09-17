#!/usr/bin/awk -f

function remove_file(file)
{
    cmd = "rm " file
    system(cmd)
    close(cmd)
}

function write_len(file, len)
{
    low_byte = and(len, 0xff)
    high_byte = and(rshift(len, 8), 0xff)
    printf "%c", low_byte > file
    printf "%c", high_byte > file
    close("file")
}

function write_crc32(write_file_out, flag, namelen, datalen, name, data)
{
    tmp_file="tmp"
    printf(flag) > tmp_file
    write_len(tmp_file, namelen)
    write_len(tmp_file, datalen)
    printf("%s", name) > tmp_file
    if (namelen % 4 != 0)
        for (j = 0; j < (4 - (namelen % 4)); j++) 
            printf("\0") > tmp_file
    printf("%s", data) > tmp_file
    if (datalen % 4 != 0)
        for (j = 0; j < (4 - (datalen % 4)); j++) 
            printf("\0") > tmp_file
    close(tmp_file)

    cmd = "crc32 " tmp_file
    cmd | getline crc_str
    retval = close(cmd)

    crc_str = "0x" crc_str
    crc = strtonum(crc_str)

    mask = 0xff
    write_byte=0
    # result = 0
    for (i = 0 ; i <= 24 ; i += 8) {
        write_byte = and(crc, mask)
        # use right shift to fetch the byte that we are going to write
        write_byte = rshift(write_byte, i)
        # FIXME, this only work with --use-lc-numeric option, or in C locale
        printf "%c", write_byte > write_file_out
        mask = lshift(mask, 8)
    }

    remove_file(tmp_file)
}

BEGIN{
    # a rarely used character to prevent seperate a line
    FS="\x07"
    
    param_arr["audio_loadaddr"]=audio_memaddr
    param_arr["dtbo_loadaddr"]="0x26400000"
    param_arr["fdt_loadaddr"]="0x02100000"
    param_arr["kernel_loadaddr"]="0x03000000"
    if ((chip=="rtd129x_spi"))
    {
        param_arr["rootfs_loadaddr"]="0x04000000"
    }
    else
    {
        param_arr["rootfs_loadaddr"]="0x02200000"
    }
    if ((chip=="rtd13xx_emmc") || (chip=="rtd13xx_spi") || (chip=="rtd13xx_nand"))
    {
        param_arr["eth_drv_para"]="fephy,bypass"
        param_arr["video_loadaddr"]="0x0f400000"
    }
	else if ((chip=="rtd16xx_emmc") || (chip=="rtd16xx_spi"))
	{
        param_arr["eth_drv_para"]="gphy,bypass,noacp"
	}
}
{
    # index() find the string "=" for the first occurrence in $1, and return the position in characters
    pos = index($1, "=")
    
    arg_val = substr($1, pos+1)
    # we substract 1 from pos so that the string "arg_idx" won't trail with "="
    arg_idx = substr($1, 0, pos-1)
    # array "param_arr" has name of sysparam as index
    param_arr[arg_idx]=arg_val
}
END{
    flag = "\0\0\0\0"
    for (param_name in param_arr) {
        namelen = length(param_name)
        datalen = length(param_arr[param_name])
        # SYSPARAM_MAGIC
        printf("PSYS") > file_out

        write_crc32(file_out,
                    flag,
                    namelen,
                    datalen,
                    param_name,
                    param_arr[param_name])
        # flag
        printf("%s", flag) > file_out
        write_len(file_out, namelen)
        write_len(file_out, datalen)
        # name & padding
        printf("%s", param_name) > file_out
        if (namelen % 4 != 0)
            for (j = 0; j < (4 - (namelen % 4)); j++) 
                printf("\0") > file_out
        # data & padding
        printf("%s", param_arr[param_name]) > file_out
        if (datalen % 4 != 0)
            for (j = 0; j < (4 - (datalen % 4)); j++)
                printf("\0") > file_out
    }
}

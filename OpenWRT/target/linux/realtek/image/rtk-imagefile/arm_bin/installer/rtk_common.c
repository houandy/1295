#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <stdarg.h>
#include <stdio.h>
#include "rtk_common.h"

extern int debug_level;
void debug_printf(int debugLevel, const char* filename, const char* funcname, int fileline, const char* fmt, ...)
{
    va_list arglist;
    if (debug_level)
    {
        switch(debugLevel&(debug_level&(~INSTALL_FAKE_RUN)))
        {
        case INSTALL_DEBUG_LEVEL:
            printf("[DEBUG][%s:%s():%d]", filename, funcname, fileline);
            break;
        case INSTALL_INFO_LEVEL:
            printf("[INFO][%s:%s():%d]", filename, funcname, fileline);
            break;
        case INSTALL_FAIL_LEVEL:
            printf("[FAIL][%s:%s():%d]", filename, funcname, fileline);
            break;
        case INSTALL_LOG_LEVEL:
            printf("[LOG][%s:%s():%d]", filename, funcname, fileline);
            break;
        case INSTALL_WARNING_LEVEL:
            printf("[WARN][%s:%s():%d]", filename, funcname, fileline);
            break;
        case INSTALL_UI_LEVEL: 
            va_start( arglist, fmt );
            vprintf( fmt, arglist);
            va_end( arglist );
            return;
        default:
            //printf("[%s:%s():%d]", filename, funcname, fileline);
            return;
            break;
        }
        va_start( arglist, fmt );
        vprintf( fmt, arglist);
        va_end( arglist );
    }
    fflush(stdout);
    fflush(stderr);    
}

int get_sizeof_file(const char* file_path, unsigned int *file_size)
{
    struct stat stat_buf = {0};
    int ret;
    ret = stat(file_path, &stat_buf);
    if(ret < 0)
    {
        install_fail("Cannot find %s\r\n", file_path);
        *file_size = 0;
        return -1;
    }
    *file_size = stat_buf.st_size;
    return 0;
}


int read_fileoffset_to_ptr(const char* filepath, unsigned int offset, void* ptr, unsigned int len)
{
    int fd;
    int byte_offset;
    fd = open(filepath, O_RDONLY);
    if(fd < 0) {
        install_fail("Cannot open %s\r\n", filepath);
        return -1;
    }
    byte_offset = lseek(fd, offset, SEEK_SET);
    if(byte_offset < 0) {
        install_fail("lseek (%s) fail\r\n", filepath);
        close(fd);
        return -1;
    }

    byte_offset = read(fd, ptr, len);
    close(fd);

    if((unsigned int)byte_offset != len) {
        install_fail("read fd fail(%d)\r\n", byte_offset);
        return -1;
    }
    return 0;
}

int write_ptr_to_fileoffset(const char* filepath, unsigned int dst_offset, void* ptr, unsigned int len)
{
    int dst_fd;
    int byte_offset;

    dst_fd = open(filepath, O_WRONLY);
    if(dst_fd < 0) {
        install_fail("Cannot open %s\r\n", filepath);
        return -1;
    }

    byte_offset = lseek(dst_fd, dst_offset, SEEK_SET);
    if(byte_offset < 0) {
        install_fail("lseek (%s) fail\r\n", filepath);
        close(dst_fd);
        return -1;
    }

    byte_offset = write(dst_fd, ptr, len);
    close(dst_fd);

    if((unsigned int)byte_offset != len) {
        install_fail("write fd fail(%d)\r\n", byte_offset);
        return -1;
    }
    else {
        install_debug("written bytes  = %d\n", byte_offset);
    }

    return 0;
}

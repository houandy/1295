#ifndef RTK_COMMON_H
#define RTK_COMMON_H

#define VT100_NONE          "\033[m"
#define VT100_RED           "\033[0;32;31m"
#define VT100_LIGHT_RED     "\033[1;31m"
#define VT100_GREEN         "\033[0;32;32m"
#define VT100_LIGHT_GREEN   "\033[1;32m"
#define VT100_BLUE          "\033[0;32;34m"
#define VT100_LIGHT_BLUE    "\033[1;34m"
#define VT100_DARY_GRAY     "\033[1;30m"
#define VT100_CYAN          "\033[0;36m"
#define VT100_LIGHT_CYAN    "\033[1;36m"
#define VT100_PURPLE        "\033[0;35m"
#define VT100_LIGHT_PURPLE  "\033[1;35m"
#define VT100_BROWN         "\033[0;33m"
#define VT100_YELLOW        "\033[1;33m"
#define VT100_LIGHT_GRAY    "\033[0;37m"
#define VT100_WHITE         "\033[1;37m"

#define INSTALL_FAIL_LEVEL  0x01
#define INSTALL_INFO_LEVEL  0x02
#define INSTALL_LOG_LEVEL       0x04
#define INSTALL_DEBUG_LEVEL  0x08
#define INSTALL_WARNING_LEVEL  0x10
#define INSTALL_UI_LEVEL  0x20
#define INSTALL_FAKE_RUN 0x80

#define install_debug(f, a...) debug_printf(INSTALL_DEBUG_LEVEL, __FILE__, __FUNCTION__, __LINE__, f,##a)
#define install_info(f, a...) debug_printf(INSTALL_INFO_LEVEL, __FILE__, __FUNCTION__, __LINE__, f, ##a)
#define install_log(f, a...) debug_printf(INSTALL_LOG_LEVEL, __FILE__, __FUNCTION__, __LINE__, f, ##a)
#define install_fail(f, a...) debug_printf(INSTALL_FAIL_LEVEL, __FILE__, __FUNCTION__, __LINE__, VT100_LIGHT_RED f VT100_NONE, ##a)
#define install_ui(f, a...) debug_printf(INSTALL_UI_LEVEL, __FILE__, __FUNCTION__, __LINE__, VT100_LIGHT_RED f VT100_NONE, ##a)
#define install_test(f, a...) debug_printf(INSTALL_DEBUG_LEVEL, __FILE__, __FUNCTION__, __LINE__, VT100_LIGHT_RED f VT100_NONE, ##a)
#define install_warn(f, a...) debug_printf(INSTALL_WARNING_LEVEL, __FILE__, __FUNCTION__, __LINE__, VT100_LIGHT_RED f VT100_NONE, ##a)

void debug_printf(int debugLevel, const char* filename, const char* funcname, int fileline, const char* fmt, ...);

int get_sizeof_file(const char* file_path, unsigned int *file_size);
int read_fileoffset_to_ptr(const char* filepath, unsigned int offset, void* ptr, unsigned int len);
int write_ptr_to_fileoffset(const char* filepath, unsigned int dst_offset, void* ptr, unsigned int len);

#endif
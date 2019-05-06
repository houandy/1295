#ifndef RTK_CONFIG_H
#define RTK_CONFIG_H
#include <string.h>
#include <stdlib.h>
#include "rtk_common.h"
static inline int del_cr_lf(char* str)
{
	char *find_cr_last = NULL;
	char *find_lf_last = NULL;

	do {
		find_cr_last = strrchr(str, '\r');
		find_lf_last = strrchr(str, '\n');

		if (find_cr_last == NULL && find_lf_last == NULL) {
			return 0;
		}
		if (find_cr_last != NULL) {
			*find_cr_last = '\0';
		}
		if (find_lf_last != NULL) {
			*find_lf_last = '\0';
		}
	}
	while(1);
}

static inline int skip_quotation(char *str, char **string, int *string_len)
{
	char *find_first = NULL, *find_last = NULL;

	find_first = strchr(str, '\"');
	find_last = strrchr(str, '\"');

	if (find_first != NULL && find_last != NULL && find_first != find_last) {
		*string = find_first + 1;
		*string_len = find_last - find_first - 1;
		return 0;
	}
	else {
		*string = str;
		*string_len = strlen(str);
		return -1;
	}
}

static inline char* skip_char(char* str, char c)
{
	do {
		if(*str==0) return NULL;
		if(*str==c) return str+1;
		str++;
	}
	while(1);
	return NULL;
}

static inline char* skip_space(char* str)
{
	do {
		if(*str==0) return NULL;
		if(*str != ' ') return str;
		str++;
	}
	while(1);
	return NULL;
}

/* parse config yes:1   no:0 */
#define create_yn_fun(var_name) \
int var_name=-1; \
int fill_yn_##var_name(char* str) \
{  \
	if(str == NULL||*str == 0) \
		return -1;  \
	if(str[0] == ' ') \
		str = skip_space(str);  \
	if((0 == strncmp(str, "y", 1)) || (0 == strncmp(str, "Y", 1)) || (0 == strncmp(str, "1", 1)))   \
	{  \
	   var_name = 1;  \
	}  \
	else  \
	{  \
	   var_name = 0;  \
	}  \
	return 0;   \
}
#define create_yn_match(var_name) \
if(0 == strncmp(#var_name, newline, strlen(#var_name))) \
{ \
	if (newline[strlen(#var_name)]=='=') \
	{ \
		fill_yn_##var_name(skip_char(newline+strlen(#var_name), '=')); \
		install_log("yn_var:%s=%d\n",#var_name, var_name); \
		continue; \
	} \
}

/* parse config int */
#define create_var_fun(var_name) \
long int var_name=-1; \
int fill_var_##var_name(char* str) \
{  \
	char *str_end = NULL; \
	if(str == NULL||*str == 0) \
		return -1;  \
	if(str[0] == ' ') \
		str = skip_space(str);  \
	if((0 == strncmp(str, "0x", 2)) || (0 == strncmp(str, "0X", 2)))   \
	{  \
	   var_name = strtol(str, &str_end, 16); \
	}  \
	else  \
	{  \
	   var_name = strtol(str, &str_end, 10); \
	}  \
	if ((var_name==LONG_MIN)||(var_name==LONG_MAX)) \
	{ \
		install_fail("%s=%s is incorrect\n", #var_name, str); \
		exit(1) ; \
	} \
	return 0;   \
}
#define create_var_match(var_name) \
if(0 == strncmp(#var_name, newline, strlen(#var_name))) \
{ 	\
	if (newline[strlen(#var_name)]=='=') \
	{ \
		fill_var_##var_name(skip_char(newline+strlen(#var_name), '=')); \
		install_log("var:%s=%d\n",#var_name, var_name); \
		continue; \
	} \
}

#endif


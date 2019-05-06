#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>

#include "c.h"
#include "nls.h"
#include "timeutils.h"
#include "alarm.h"
#include "xalloc.h"
#include "rtk_factory.h"
#include "rtk_common.h"
#include "rtk_layout.h"

#define FACTORY_DIR "/tmp/factory"
#define FACTORY_ENV_FILE "/tmp/factory/env.txt"
#define ENV_SIZE ENV_TOTAL_SIZE

void usage(char *filename)
{

    printf("usage:\t%s (load|set|save|list) [variable] [value]\n", filename);
    printf("\t\tSet without value to unset the env variable.\n");
    printf("\t\tYou could do multiple set command before save.\n");
    printf("usage:\t%s (flag|unflag) FLAGNAME\n", filename);
    printf("\t\tFLAGNAME could be ACRECOVERY or SHUTDOWN\n");
    printf("usage:\t%s alarm (set|read) [timestamp]\n", filename);
    printf("\t\tSet without timestamp to unset alarm.\n");
}

int printenv(bool print, char *env_par, char *env_val)
{
    FILE *fp=NULL;
    unsigned int crc;
    char name[511];
    int index = 0;
    int findex = 0;
    bool isSet = false;
    char *buff;
    unsigned int env_size = ENV_SIZE;
    struct stat env;

    // error handling
    if (env_par && strchr(env_par, '='))
    {
        printf("Invalid variable name!\n");
        return -1;
    }

    if (env_par && env_val)
    {
        if(strlen(env_par)+strlen(env_val) > 510)
        {
            printf("Env value is large!\n");
            return -1;
        }
    }

    /* Get env.txt size from original file */
    if (!stat(FACTORY_ENV_FILE, &env))
    {
        env_size = (unsigned int)env.st_size;
    }

    fp = fopen(FACTORY_ENV_FILE, "rb+");
    fread(&crc, 4, 1, fp);
    if(print)
        printf("CRC is %08x\n===============\n", crc);
    findex = 4;


    buff = (char*)malloc(env_size);
    memset(buff, 0, env_size);
    u_int8_t *envptr = (u_int8_t *) buff + 4;
    while(fread(&name[index], 1, 1, fp) > 0)
    {
        if(name[index] == '\0')
        {
            if(index != 0)
            {
                if(env_par){
                    if(!strncmp(name, env_par, strlen(env_par)) && '=' == name[strlen(env_par)])
                    {
                        isSet = true;
                        if(env_val){
                        memcpy(name+strlen(env_par)+1, env_val, strlen(env_val)+1);
                        }
                        else{
                            // remove this env variable
                            index = 0;
                            continue;
                        }
                    }
                }
                if(print)
                    printf("%s\n", name);
                // error handling
                if(findex+strlen(name)+1 > env_size)
                {
                    printf("Env content(%zu) is larger than MAX(%d)!\n",
                           findex+strlen(name)+1, env_size);
                    free((void*)buff);
                    return -1;
                }
                memcpy(buff+findex, name, strlen(name)+1);
                findex += strlen(name)+1;
            }
            index = 0;
        }
        else index++;
    }
    // Set for new env
    if(!isSet && !print)
    {
        memcpy(name, env_par, strlen(env_par));
        name[strlen(env_par)] = '=';
        memcpy(name+strlen(env_par)+1, env_val, strlen(env_val)+1);
        // error handling
        if(findex+strlen(name)+1 > env_size)
        {
            printf("Env content(%zu) is larger than MAX(%d)!\n",
                   findex+strlen(name)+1, env_size);
            free((void*)buff);
            return -1;
        }

        memcpy(buff+findex, name, strlen(name)+1);
    }

    fseek (fp, 0, SEEK_SET);
    crc32(envptr, env_size - 4, &crc);
    if(print)
        printf("===============\nUpdated CRC is %08x\n", crc);
    memcpy(buff, &crc, sizeof(crc));
    fwrite(buff, env_size, 1, fp);
    fflush(fp);
    fsync(fileno(fp));
    fclose(fp);
    free((void*)buff);
    return 0;
}

#define RTC_DEVICE      "/dev/rtc0"
#define POWER_UP_RTC_PATH FACTORY_DIR"/RTC"
int readalarm()
{
    int fd;
    struct rtcwake_control ctl = { 0 };
    int rc = EXIT_SUCCESS;

    /* Read alarm from RTC */
    fd = open_dev_rtc(RTC_DEVICE);
    if (print_alarm(&ctl, fd))
        return EXIT_FAILURE;

    /* Read alarm from factory file if exists */
    if(!access(POWER_UP_RTC_PATH, F_OK)) {
        struct rtc_time record = { 0 };
        FILE *fp;

        fp = fopen(POWER_UP_RTC_PATH, "rb");
        if(!fp) {
            printf("Failed to load factory RTC value\n");
            return EXIT_FAILURE;
        }
        fread(&record, 1, sizeof(struct rtc_time), fp);
        fclose(fp);

        printf("Factory RTC: %04d-%02d-%02d %02d:%02d:%02d\n",
          record.tm_year + 1900, record.tm_mon + 1, record.tm_mday,
          record.tm_hour, record.tm_min, record.tm_sec);
    }

    return rc;
}

int setalarm(char* timestamp)
{
    int fd;
    struct rtcwake_control ctl = { 0 };
    int rc = EXIT_SUCCESS;
    time_t alarm = 0;
    usec_t p;
    struct rtc_wkalrm wake = { 0 };
    int save = 0;

    fd = open_dev_rtc(RTC_DEVICE);
    if(timestamp) {
        if (parse_timestamp(timestamp, &p) < 0) {
            printf("invalid time value \"%s\"\n", timestamp);
            return EXIT_FAILURE;
        }
        alarm = (time_t) (p / 1000000);

        if (setup_alarm(&ctl, fd, &alarm) < 0)
            rc = EXIT_FAILURE;
    }
    else {
        if (ioctl(fd, RTC_WKALM_RD, &wake) < 0) {
            warn(_("read rtc alarm failed"));
            rc = EXIT_FAILURE;
        } else {
            unsigned char enabled = wake.enabled;
            wake.enabled = 0;
            if (ioctl(fd, RTC_WKALM_SET, &wake) < 0) {
                warn(_("disable rtc alarm interrupt failed"));
                rc = EXIT_FAILURE;
            }
            if(!access(POWER_UP_RTC_PATH, F_OK)) {
                unlink(POWER_UP_RTC_PATH);
                save = 1;
            }

            if(enabled) {
                printf("Unset alarm successfully\n");
                printf("Unset alarm: %04d-%02d-%02d %02d:%02d:%02d\n",
                  wake.time.tm_year + 1900, wake.time.tm_mon + 1, wake.time.tm_mday,
                  wake.time.tm_hour, wake.time.tm_min, wake.time.tm_sec);
            }
            else {
                printf("No alarm set\n");
            }
        }
    }

    /* Save next RTC alarm to factory RTC file */
    /* RAW format:
     * tm_year : offset from 1900
     * tm_mon : count from 0~11
     * tm_sec : not available
     */
    if (!ioctl(fd, RTC_WKALM_RD, &wake) && 1 == wake.enabled ) {
        FILE *fp;

        fp = fopen(POWER_UP_RTC_PATH, "wb");
        if(!fp) {
            printf("Failed to write factory RTC value\n");
            return EXIT_FAILURE;
        }
        fwrite(&wake.time, 1, sizeof(struct rtc_time), fp);
        fclose(fp);
        save = 1;

        printf("Save RTC to factory: %04d-%02d-%02d %02d:%02d:%02d\n",
          wake.time.tm_year + 1900, wake.time.tm_mon + 1, wake.time.tm_mday,
          wake.time.tm_hour, wake.time.tm_min, wake.time.tm_sec);
    }

    close(fd);

    if(save)
        factory_flush(0, 0);

    return rc;
}

int check_if_rtc_exist()
{
    int fd;
    char *devpath = NULL;
    int rc = EXIT_SUCCESS;
    
	devpath = xstrdup(RTC_DEVICE);

	fd = open(devpath, O_RDONLY | O_CLOEXEC);
	if (fd < 0) {
        printf("%s: unable to find device\n", devpath);
        rc = EXIT_FAILURE;
    }
    free(devpath);
    return rc;
}

int main(int argc, char **argv)
{
    int ret = 0;
    char path[511] = {0};

    printf("argc = %d\n", argc);
    printf("argv = %s %s\n", argv[0], argv[1]);

    if( !(argc == 2 || argc == 4 \
          || (argc==3 && !strcmp(argv[1], "set")) \
          || (argc==3 && (!strcmp(argv[1], "flag")||!strcmp(argv[1], "unflag"))) \
          || (argc >= 3 && !strcmp(argv[1], "alarm")) \
          )\
      )
    {
        usage(argv[0]);
        return -1;
    }
    if(!strcmp(argv[1], "load"))
    {
        if(rtk_file_lock()) return -1;
        factory_load();
        if(argc != 2)
            ret = printenv(true, argv[2], argv[3]);
        if(rtk_file_unlock()) return -1;
    }
    else if(!strcmp(argv[1], "set"))
    {
        if(rtk_file_lock()) return -1;
        if(access(FACTORY_ENV_FILE, F_OK))
            factory_load();
        if(access(FACTORY_ENV_FILE, F_OK))
        {
            rtk_file_unlock();
            return -1;
        }
        if(argc == 4 || argc == 3)
            ret = printenv(false, argv[2], argv[3]);
        if(rtk_file_unlock()) return -1;
    }
    else if(!strcmp(argv[1], "save"))
    {
        if(rtk_file_lock()) return -1;
        factory_flush(0, 0);
        if(rtk_file_unlock()) return -1;
    }
    else if (!strcmp(argv[1], "list"))
    {
        if(rtk_file_lock()) return -1;
        if(access(FACTORY_ENV_FILE, F_OK))
            factory_load();
        printenv(true, NULL, NULL);
        if (rtk_file_unlock()) return -1;
    }
    else if (argc==3 && (!strcmp(argv[1], "flag")||!strcmp(argv[1], "unflag"))) \
    {
        if(rtk_file_lock()) return -1;
        if(access(FACTORY_DIR, F_OK))
            factory_load();

        if (access(FACTORY_DIR, F_OK) == 0) {
            struct stat flag;
            sprintf(path, "%s/%s", FACTORY_DIR, argv[2]);
            if(!strcmp(argv[1], "flag") && access(path, F_OK)) {
                close(open(path, O_WRONLY|O_CREAT|O_CLOEXEC, 0000));
                factory_flush(0, 0);
            }
            else if (!strcmp(argv[1], "unflag") && !stat(path, &flag) && 0 == flag.st_size) {
                unlink(path);
                factory_flush(0, 0);
            }
        }

        if (rtk_file_unlock()) return -1;
    }
    else if (!strcmp(argv[1], "alarm"))
    {
        if(argc == 4 || argc == 3)
        {
            if(rtk_file_lock()) return -1;
            if(access(FACTORY_DIR, F_OK)) {
                /* access factory dir fail */
                factory_load();
            }
            if (access(FACTORY_DIR, F_OK) == 0) {
                /* access factory dir successfully after factory_load */
                if(check_if_rtc_exist() == 0) {
                    if(!strcmp(argv[2], "read"))
                        ret = readalarm();
                    else if(!strcmp(argv[2], "set"))
                        ret = setalarm(argv[3]);
                }
            }
            if (rtk_file_unlock()) return -1;
        }
    }

    return ret;
}


/*
* Copyright (C) 2015-2017 Realtek
*
* This is free software, licensed under the GNU General Public License v2.
* See /LICENSE for more information.
*
*/

#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <net/if.h>
#include <netinet/ip.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define CMDLINE_SIZE 2048
#define CMD_SIZE 128
#define NETNS_RUN_DIR "/var/run/netns"
#define ANDROID_PROP "./default.prop ./init*.rc"
#define OTT_WIFI "./.ottwifi"

enum rtk_init_mode {
    OTT_MODE,
    ROUTER_MODE,
};

static const char *hw_list[] = {"thor", "kylin"};
static const char *android_root = "/android";
static char *path_string = "PATH=/usr/sbin:/usr/bin:/sbin:/bin";
static const char *ifname = "eth0";
static const char *androidnet = "androidnet";
static const char *oifname = "veth1";
static const char *aifname = "eth9";

static char cmdline[CMDLINE_SIZE + 1] = { 0 };

char* get_cmdline_val(const char* name, char* out, int len)
{
    char *c, *sptr;

    if (!cmdline[0]){
        int fd;
        ssize_t r;

        mount("proc", "/proc", "proc", MS_NOATIME | MS_NODEV | MS_NOEXEC | MS_NOSUID, 0);
        fd = open("/proc/cmdline", O_RDONLY);
        r = read(fd, cmdline, sizeof(cmdline) - 1);
        close(fd);
        umount("/proc");

        if (r <= 0)
            return NULL;
        cmdline[r] = 0;
    }

    if (!out) return NULL;
    out[0] = '\0';

    for (c = strtok_r(cmdline, " \t\n", &sptr); c;
                    c = strtok_r(NULL, " \t\n", &sptr)) {
        char *sep = strchr(c, '=');
        ssize_t klen = sep - c;

        // Restore null-terminated token
        c[strlen(c)] = ' ';
        if (klen < 0 || strncmp(name, c, klen) || name[klen] != 0)
            continue;

        strncpy(out, &sep[1], len);
        out[len-1] = 0;
        // Continue to find the next one
        continue;
    }

    if (out && out[0]) return out;

    return NULL;
}

int check_hw_support(const char *hw)
{
    int i;
    int ret = 1;
    int len = sizeof(hw_list)/sizeof(char*);

    for (i=0; i<len; i++) {
        if (!strncmp(hw, hw_list[i], strlen(hw_list[i]))) {
            ret = 0;
            break;
        }
    }

    return ret;
}

int main (int argc, char **argv)
{
    struct stat s;
    int ret = 0;
    const char *value;
    char line[CMD_SIZE];

    value = get_cmdline_val("androidboot.hardware", line, sizeof(line));

    // Use /sbin/init instead of /etc/preinit
    //if (stat("/mnt/android", &s) || stat(android_root, &s)) {
    if (!value || check_hw_support(value)) {
        fprintf(stderr, "===== OpenWRT =====\n");
        // Exec /sbin/init in pure OpenWRT environment
        argv[0] = "/sbin/init";
        ret = execv("/sbin/init", argv);
    }
    else {
        fprintf(stderr, "===== OpenWRT + Android =====\n");
        pid_t child;
        int status, count = 0;
        enum rtk_init_mode mode = OTT_MODE;
        char cmd[CMD_SIZE] = {0};

        if (stat("/mnt/android/.ottwifi", &s)) {
            mode = ROUTER_MODE;
            fprintf(stderr, "===== Router mode =====\n");
        }

        child = fork();
        // Exec OpenWRT init
        if (0 == child) {
            setsid();
            //exec unshare --fork --pid /sbin/init $*
            unshare(CLONE_NEWPID);
            child = fork();
            if (0 == child) {
                argv[0] = "/sbin/init";
                execv("/sbin/init", argv);
                _exit(EXIT_FAILURE);
            }
            else {
                waitpid(child, &status, 0);
                exit(0);
            }
        }

        else if (child < 0) {
            return -1;
        }

        do {
            // boot in 2-in-1 or 3-in-1
            mount("tmpfs", android_root, "tmpfs", MS_NOATIME, "size=10240K");
            putenv(path_string);
            snprintf(cmd, CMD_SIZE, "cp -a /mnt/android/* %s", android_root);
            // TODO: Chroot here and skip ip command
            if( (ret = system(cmd) < 0) \
                || (ret = chdir(android_root) < 0)
              //  || (ret = chroot(".") < 0)
              )
                break;

            // Detect console settings
            value = get_cmdline_val("console.switch", line, sizeof(line));
            if (value && !strncmp(value, "android", strlen("android"))) {
                snprintf(cmd, CMD_SIZE, "sed -i 's,\\(ro.debuggable=\\)\\(.*\\),\\11,g' %s", ANDROID_PROP);
            }
            else {
                snprintf(cmd, CMD_SIZE, "sed -i 's,\\(ro.debuggable=\\)\\(.*\\),\\10,g' %s", ANDROID_PROP);
            }
            system(cmd);

            // Block and wait for OpenWRT coldplug up to 10 seconds
            while(stat(".coldplug_done", &s) && count++ < 100) {
                usleep(100*1000);
            }

            if (ROUTER_MODE == mode) {
                struct ifreq ifr = {0};
                int sd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
                unsigned char hwaddr[6] = {0x00, 0xe0, 0x4c, 0x0b, 0xda, 0x05};
                unsigned char hwaddr_str[18] = {0};
                int netfd;

                strncpy(ifr.ifr_name, ifname, strlen(ifname));
                // Get last 4 digit of eth0 addr
                if ( sd >=0 && 0 == ioctl(sd, SIOCGIFHWADDR, &ifr)) {
                    hwaddr[4] = (unsigned char) ifr.ifr_addr.sa_data[4];
                    hwaddr[5] = (unsigned char) ifr.ifr_addr.sa_data[5];
                    close(sd);
                }

                /* TODO: Use RTNETLINK kernel communication */
                mount("proc", "/proc", "proc", MS_NOATIME | MS_NODEV | MS_NOEXEC | MS_NOSUID, 0);
                mkdir("/var/run/netns", 0755);
                snprintf(cmd, CMD_SIZE, "ip netns add %s", androidnet);
                system(cmd);
                //ip link add veth1 addr 00:e0:4c:0b:${mac_suffix} type veth peer name eth9 addr 00:e0:4c:ab:${mac_suffix}
                snprintf(hwaddr_str, sizeof(hwaddr_str),
                  "%02x:%02x:%02x:%02x:%02x:%02x",
                  hwaddr[0], hwaddr[1], hwaddr[2], hwaddr[3], hwaddr[4], hwaddr[5]);
                snprintf(cmd, CMD_SIZE, "ip link add %s addr %s type veth peer name %s addr ",
                         oifname, hwaddr_str, aifname);
                hwaddr_str[9] = 'a';
                strncat(cmd, hwaddr_str, strlen(hwaddr_str));
                system(cmd);
                snprintf(cmd, CMD_SIZE, "ip link set %s netns %s", aifname, androidnet);
                system(cmd);
                snprintf(cmd, CMD_SIZE, "ip netns exec %s ip link set dev lo up", androidnet); 
                system(cmd);

                //nsenter --net=/var/run/netns/androidnet
                snprintf(cmd, CMD_SIZE, "%s/%s", NETNS_RUN_DIR, androidnet);
                netfd = open(cmd, O_RDONLY);
                ret = setns(netfd, CLONE_NEWNET);
                close(netfd);
                umount("/proc");
            }
            else {
                close(open(OTT_WIFI, O_WRONLY|O_CREAT|O_CLOEXEC, 0000));
            }
            unshare(CLONE_NEWNS);
            // TODO: Skip this
            ret = chroot(".");
            argv[0] = "/init";
            ret = execv("/init", argv);
        } while(0);
    }

    return ret;
}

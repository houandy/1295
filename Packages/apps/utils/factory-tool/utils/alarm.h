#ifndef UTIL_LINUX_ALARM_H
#define UTIL_LINUX_ALARM_H

enum clock_modes {
	CM_AUTO,
	CM_UTC,
	CM_LOCAL
};

struct rtcwake_control {
	char *mode_str;			/* name of the requested mode */
	char **possible_modes;		/* modes listed in /sys/power/state */
	char *adjfile;			/* adjtime file path */
	enum clock_modes clock_mode;	/* hwclock timezone */
	time_t sys_time;		/* system time */
	time_t rtc_time;		/* hardware time */
	unsigned int verbose:1,		/* verbose messaging */
		     dryrun:1;		/* do not set alarm, suspend system, etc */
};

#ifdef __cplusplus
extern "C" {
#endif

int is_wakeup_enabled(const char *devname);
int open_dev_rtc(const char *devname);

int setup_alarm(struct rtcwake_control *ctl, int fd, time_t *wakeup);
int print_alarm(struct rtcwake_control *ctl, int fd);


#ifdef __cplusplus
}
#endif

#endif

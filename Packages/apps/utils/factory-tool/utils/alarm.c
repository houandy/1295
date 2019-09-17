/*
 * rtcwake -- enter a system sleep state until specified wakeup time.
 *
 * This uses cross-platform Linux interfaces to enter a system sleep state,
 * and leave it no later than a specified time.  It uses any RTC framework
 * driver that supports standard driver model wakeup flags.
 *
 * This is normally used like the old "apmsleep" utility, to wake from a
 * suspend state like ACPI S1 (standby) or S3 (suspend-to-RAM).  Most
 * platforms can implement those without analogues of BIOS, APM, or ACPI.
 *
 * On some systems, this can also be used like "nvram-wakeup", waking
 * from states like ACPI S4 (suspend to disk).  Not all systems have
 * persistent media that are appropriate for such suspend modes.
 *
 * The best way to set the system's RTC is so that it holds the current
 * time in UTC.  Use the "-l" flag to tell this program that the system
 * RTC uses a local timezone instead (maybe you dual-boot MS-Windows).
 * That flag should not be needed on systems with adjtime support.
 */

#include <errno.h>
#include <linux/rtc.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#include "c.h"
#include "nls.h"
#include "strutils.h"
#include "xalloc.h"

#include "alarm.h"

#define SYS_WAKEUP_PATH_TEMPLATE	"/sys/class/rtc/%s/device/power/wakeup"
#define SYS_POWER_STATE_PATH		"/sys/power/state"
#define DEFAULT_RTC_DEVICE		"/dev/rtc0"

int is_wakeup_enabled(const char *devname)
{
	char	buf[128], *s;
	FILE	*f;
	size_t	skip = 0;

	if (startswith(devname, "/dev/"))
		skip = 5;
	snprintf(buf, sizeof buf, SYS_WAKEUP_PATH_TEMPLATE, devname + skip);
	f = fopen(buf, "r");
	if (!f) {
		warn(_("cannot open %s"), buf);
		return 0;
	}

	s = fgets(buf, sizeof buf, f);
	fclose(f);
	if (!s)
		return 0;
	s = strchr(buf, '\n');
	if (!s)
		return 0;
	*s = 0;
	/* wakeup events could be disabled or not supported */
	return strcmp(buf, "enabled") == 0;
}

static int get_basetimes(struct rtcwake_control *ctl, int fd)
{
	struct tm tm = { 0 };
	struct rtc_time	rtc;

	/* This process works in RTC time, except when working
	 * with the system clock (which always uses UTC).
	 */
	if (ctl->clock_mode == CM_UTC)
		setenv("TZ", "UTC", 1);
	tzset();
	/* Read rtc and system clocks "at the same time", or as
	 * precisely (+/- a second) as we can read them.
	 */
	if (ioctl(fd, RTC_RD_TIME, &rtc) < 0) {
		warn(_("read rtc time failed"));
		return -1;
	}

	ctl->sys_time = time(0);
	if (ctl->sys_time == (time_t)-1) {
		warn(_("read system time failed"));
		return -1;
	}
	/* Convert rtc_time to normal arithmetic-friendly form,
	 * updating tm.tm_wday as used by asctime().
	 */
	tm.tm_sec = rtc.tm_sec;
	tm.tm_min = rtc.tm_min;
	tm.tm_hour = rtc.tm_hour;
	tm.tm_mday = rtc.tm_mday;
	tm.tm_mon = rtc.tm_mon;
	tm.tm_year = rtc.tm_year;
	tm.tm_isdst = -1;  /* assume the system knows better than the RTC */

	ctl->rtc_time = mktime(&tm);
	if (ctl->rtc_time == (time_t)-1) {
		warn(_("convert rtc time failed"));
		return -1;
	}

	if (ctl->verbose) {
		/* Unless the system uses UTC, either delta or tzone
		 * reflects a seconds offset from UTC.  The value can
		 * help sort out problems like bugs in your C library. */
		printf("\tdelta   = %ld\n", ctl->sys_time - ctl->rtc_time);
		printf("\ttzone   = %ld\n", timezone);
		printf("\ttzname  = %s\n", tzname[daylight]);
		gmtime_r(&ctl->rtc_time, &tm);
		printf("\tsystime = %ld, (UTC) %s",
				(long) ctl->sys_time, asctime(gmtime(&ctl->sys_time)));
		printf("\trtctime = %ld, (UTC) %s",
				(long) ctl->rtc_time, asctime(&tm));
	}
	return 0;
}

int setup_alarm(struct rtcwake_control *ctl, int fd, time_t *wakeup)
{
	struct tm		*tm;
	struct rtc_wkalrm	wake = { 0 };

	/* The wakeup time is in POSIX time (more or less UTC).  Ideally
	 * RTCs use that same time; but PCs can't do that if they need to
	 * boot MS-Windows.  Messy...
	 *
	 * When clock_mode == CM_UTC this process's timezone is UTC, so
	 * we'll pass a UTC date to the RTC.
	 *
	 * Else clock_mode == CM_LOCAL so the time given to the RTC will
	 * instead use the local time zone. */
	tm = localtime(wakeup);
	wake.time.tm_sec = tm->tm_sec;
	wake.time.tm_min = tm->tm_min;
	wake.time.tm_hour = tm->tm_hour;
	wake.time.tm_mday = tm->tm_mday;
	wake.time.tm_mon = tm->tm_mon;
	wake.time.tm_year = tm->tm_year;
	/* wday, yday, and isdst fields are unused */
	wake.time.tm_wday = -1;
	wake.time.tm_yday = -1;
	wake.time.tm_isdst = -1;
	wake.enabled = 1;

	if (!ctl->dryrun && ioctl(fd, RTC_WKALM_SET, &wake) < 0) {
		warn(_("set rtc wake alarm failed"));
		return -1;
	}
	return 0;
}

int print_alarm(struct rtcwake_control *ctl, int fd)
{
	struct rtc_wkalrm wake;
	struct tm tm = { 0 };
	time_t alarm;

	if (ioctl(fd, RTC_WKALM_RD, &wake) < 0) {
		warn(_("read rtc alarm failed"));
		return -1;
	}

	if (wake.enabled != 1 || wake.time.tm_year == -1) {
		printf(_("alarm: off\n"));
		return 0;
	}
	tm.tm_sec = wake.time.tm_sec;
	tm.tm_min = wake.time.tm_min;
	tm.tm_hour = wake.time.tm_hour;
	tm.tm_mday = wake.time.tm_mday;
	tm.tm_mon = wake.time.tm_mon;
	tm.tm_year = wake.time.tm_year;
	tm.tm_isdst = -1;  /* assume the system knows better than the RTC */

	alarm = mktime(&tm);
	if (alarm == (time_t)-1) {
		warn(_("convert time failed"));
		return -1;
	}
	/* 0 if both UTC, or expresses diff if RTC in local time */
	alarm += ctl->sys_time - ctl->rtc_time;
	printf(_("alarm: on  %s"), ctime(&alarm));

	return 0;
}

int open_dev_rtc(const char *devname)
{
	int fd;
	char *devpath = NULL;

	if (startswith(devname, "/dev"))
		devpath = xstrdup(devname);
	else
		xasprintf(&devpath, "/dev/%s", devname);
	fd = open(devpath, O_RDONLY | O_CLOEXEC);
	if (fd < 0)
		err(EXIT_FAILURE, _("%s: unable to find device"), devpath);
	free(devpath);
	return fd;
}

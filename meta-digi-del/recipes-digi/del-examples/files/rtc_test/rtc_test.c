/*
 * rtc_test.c
 *
 * Copyright (C) 2009-2013 by Digi International Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * Description: RTC test application
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>

#define	PROGRAM			"rtc_test"
#define VERSION			"2.0"

#define	RTC_DEVICE_FILE		"/dev/rtc"

/* test options */
#define	RTC_TEST_RD_TIME	(1<<0)
#define	RTC_TEST_SET_TIME	(1<<1)
#define	RTC_TEST_RD_ALARM	(1<<2)
#define	RTC_TEST_SET_ALARM	(1<<3)
#define	RTC_TEST_ALARM_IRQ	(1<<4)

#define	RTC_DEFAULT_TEST_OPS	0	/* None, pass it through the command line */

#if defined (DEL_CCWMX53JS) || defined (DEL_CCMX53JS)
#define RTC_ALARM_SECS 60
#define RTC_ALARM_SECS_STR "60"
#else
#define RTC_ALARM_SECS 5
#define RTC_ALARM_SECS_STR "5"
#endif

#define rtc_test_usage \
	"[-abcdeh]\n"
#define rtc_test_full_usage \
	"rtc_test [options]\n\n" \
        "Tests the real time clock driver\n" \
        "Options:\n" \
        "  -a : Read the current time\n" \
        "  -b : Set the current time\n" \
        "  -c : Read the alarm programed value\n" \
        "  -d : Set the alarm to trigger in " RTC_ALARM_SECS_STR " seconds\n" \
        "  -e : Test the alarm interrupt\n" \
        "  -h : Help\n\n"

/* Function prototypes */
static void rtc_test_banner(void);
static void exit_error(char *error_msg, int exit_val);
static void show_usage_exit(int exit_val, int full);
static int rtc_test_time_read(int fd, struct rtc_time *tm);
static int rtc_test_time_set(int fd, struct rtc_time *tm);
static int rtc_test_alarm_read(int fd);
static int rtc_test_alarm_set(int fd, struct rtc_time *tm);
static int rtc_test_alarm_irq(int fd, struct rtc_wkalrm *wkalrm);
static void rtc_test_display_test_results(unsigned int test_ops, unsigned int test_results);

/*
 * Function:    rtc_test_banner
 * Description: print message
 */
static void rtc_test_banner(void)
{
	fprintf(stdout, "%s %s Copyright Digi International Inc.\n\n"
		"RTC test/demo application\n\n", PROGRAM, VERSION);
}

/*
 * Function:    exit_error
 * Description: print error message and exit
 */
static void exit_error(char *error_msg, int exit_val)
{
	if (error_msg != NULL)
		fprintf(stderr, "%s", error_msg);

	exit(exit_val);
}

/*
 * Function:    show_usage_exit
 * Description: print usage information and exit
 */
static void show_usage_exit(int exit_val, int full)
{
	if (full) {
		rtc_test_banner();
		fprintf(stdout, "%s", rtc_test_full_usage);
	} else {
		fprintf(stdout, "%s", rtc_test_usage);
	}

	exit_error(NULL, exit_val);
}

/*
 * Function:    main
 * Description: application's main function
 */
int main(int argc, char *argv[])
{
	int opt, rtc_fd, retval;
	unsigned int test_ops = RTC_DEFAULT_TEST_OPS;
	unsigned int test_results = 0;
	struct rtc_time rtc_tm;
	struct rtc_wkalrm wkalrm;

	memset(&rtc_tm, 0, sizeof(struct rtc_time));
	memset(&wkalrm, 0, sizeof(struct rtc_wkalrm));

	if (argc > 1) {
		while ((opt = getopt(argc, argv, "abcdeh")) > 0) {
			switch (opt) {
			case 'a':
				test_ops |= RTC_TEST_RD_TIME;
				break;
			case 'b':
				test_ops |= RTC_TEST_SET_TIME;
				break;
			case 'c':
				test_ops |= RTC_TEST_RD_ALARM;
				break;
			case 'd':
				test_ops |= RTC_TEST_SET_ALARM;
				break;
			case 'e':
				test_ops |= RTC_TEST_ALARM_IRQ;
				break;
			case 'h':
			default:
				show_usage_exit((opt == 'h') ? EXIT_SUCCESS : EXIT_FAILURE, 1);
			}
		}
	}

	rtc_test_banner();

	rtc_fd = open(RTC_DEVICE_FILE, O_RDONLY);
	if (rtc_fd < 0) {
		perror(RTC_DEVICE_FILE);
		exit(EXIT_FAILURE);
	}

	if (test_ops &
	    (RTC_TEST_RD_TIME | RTC_TEST_SET_ALARM | RTC_TEST_ALARM_IRQ | RTC_TEST_SET_TIME)) {
		retval = rtc_test_time_read(rtc_fd, &rtc_tm);
		if (retval == 1)
			test_results |= RTC_TEST_RD_TIME;
	}

	if (test_ops & RTC_TEST_SET_TIME) {
		retval = rtc_test_time_set(rtc_fd, &rtc_tm);
		if (retval == 1)
			test_results |= RTC_TEST_SET_TIME;
	}

	if (test_ops & RTC_TEST_RD_ALARM) {
		retval = rtc_test_alarm_read(rtc_fd);
		if (retval == 1)
			test_results |= RTC_TEST_RD_ALARM;
	}

	if (test_ops & (RTC_TEST_SET_ALARM | RTC_TEST_ALARM_IRQ)) {
		retval = rtc_test_alarm_set(rtc_fd, &rtc_tm);
		if (retval == 1)
			test_results |= RTC_TEST_SET_ALARM;
	}

	if (test_ops & RTC_TEST_ALARM_IRQ) {
		memcpy(&wkalrm.time, &rtc_tm, sizeof(struct rtc_time));
		wkalrm.enabled = 1;
		retval = rtc_test_alarm_irq(rtc_fd, &wkalrm);
		if (retval == 1)
			test_results |= RTC_TEST_ALARM_IRQ;
		else if (retval == 0)
			test_results &= ~RTC_TEST_ALARM_IRQ;
	}

	rtc_test_display_test_results(test_ops, test_results);

	close(rtc_fd);
	printf("\nTest finished\n");

	return 0;
}

/*
 * Function:    rtc_test_display_test_results
 * Description: display test's results
 */
static void rtc_test_display_test_results(unsigned int test_ops, unsigned int test_results)
{
	if (test_ops) {
		printf("\nTest results:\n");
		printf("-----------------------------------------------------------\n");
	}

	if (test_ops & RTC_TEST_RD_TIME)
		printf("ioctl cmd RTC_RD_TIME:         %s\n",
		       test_results & RTC_TEST_RD_TIME ? "OK" : "Failure or not supported");
	if (test_ops & RTC_TEST_SET_TIME)
		printf("ioctl cmd RTC_SET_TIME:        %s\n",
		       test_results & RTC_TEST_SET_TIME ? "OK" : "Failure or not supported");
	if (test_ops & RTC_TEST_RD_ALARM)
		printf("ioctl cmd RTC_ALM_READ:        %s\n",
		       test_results & RTC_TEST_RD_ALARM ? "OK" : "Failure or not supported");
	if (test_ops & RTC_TEST_SET_ALARM)
		printf("ioctl cmd RTC_ALM_SET:         %s\n",
		       test_results & RTC_TEST_SET_ALARM ? "OK" : "Failure or not supported");
	if (test_ops & RTC_TEST_ALARM_IRQ)
		printf("Alarm interrupt test:          %s\n",
		       test_results & RTC_TEST_ALARM_IRQ ? "OK" : "Failure or not supported");
}

/*
 * Function:    rtc_test_time_read
 * Description: read time from the rtc
 */
static int rtc_test_time_read(int fd, struct rtc_time *tm)
{
	int retval;

	retval = ioctl(fd, RTC_RD_TIME, tm);
	if (retval >= 0) {
		printf("Current RTC date/time is %d-%d-%d, %02d:%02d:%02d.\n",
		       tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900,
		       tm->tm_hour, tm->tm_min, tm->tm_sec);
		return 1;
	}

	return 0;
}

/*
 * Function:    rtc_test_time_set
 * Description: set time into the rtc
 */
static int rtc_test_time_set(int fd, struct rtc_time *tm)
{
	int retval;

	tm->tm_sec += 2;
	if (tm->tm_sec >= 60) {
		tm->tm_sec %= 60;
		tm->tm_min++;
	}
	if (tm->tm_min == 60) {
		tm->tm_min = 0;
		tm->tm_hour++;
	}
	if (tm->tm_hour == 24)
		tm->tm_hour = 0;

	retval = ioctl(fd, RTC_SET_TIME, tm);
	if (retval >= 0) {
		printf("New RTC date/time is %d-%d-%d, %02d:%02d:%02d.\n",
		       tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900,
		       tm->tm_hour, tm->tm_min, tm->tm_sec);
		return 1;
	}

	return 0;
}

/*
 * Function:    rtc_test_alarm_read
 * Description: read the programmed time into the rtc alarm
 */
static int rtc_test_alarm_read(int fd)
{
	int retval;
	struct rtc_time alarm_tm;

	retval = ioctl(fd, RTC_ALM_READ, &alarm_tm);
	if (retval >= 0) {
		printf("Alarm was programmed to %02d:%02d:%02d.\n",
		       alarm_tm.tm_hour, alarm_tm.tm_min, alarm_tm.tm_sec);
		return 1;
	}

	return 0;
}

/*
 * Function:    rtc_test_alarm_set
 * Description: set time into rtc's alarm
 */
static int rtc_test_alarm_set(int fd, struct rtc_time *tm)
{
	int retval;
	struct rtc_time alarm_tm;
	tm->tm_sec += RTC_ALARM_SECS;
	if (tm->tm_sec >= 60) {
		tm->tm_sec %= 60;
		tm->tm_min++;
	}
	if (tm->tm_min == 60) {
		tm->tm_min = 0;
		tm->tm_hour++;
	}
	if (tm->tm_hour == 24)
		tm->tm_hour = 0;

	retval = ioctl(fd, RTC_ALM_SET, tm);
	if (retval >= 0) {
		/* Enable alarm interrupts */
		retval = ioctl(fd, RTC_AIE_ON, tm);
		if (retval >= 0 ) {
			retval = ioctl(fd, RTC_ALM_READ, &alarm_tm);
			if (retval >= 0) {
				printf("Alarm re-programmed to %02d:%02d:%02d.\n",
				       alarm_tm.tm_hour, alarm_tm.tm_min, alarm_tm.tm_sec);
				return 1;
			}
		}
	}

	return 0;
}

/*
 * Function:    rtc_test_alarm_irq
 * Description: check the irq alarm
 */
static int rtc_test_alarm_irq(int fd, struct rtc_wkalrm *wkalrm)
{
	int retval, result = 0;
	unsigned long data;
	struct timeval tv = { RTC_ALARM_SECS + 2, 0 };	/* RTC_ALARM_SECS + 2 second timeout on select */
	fd_set readfds;

	/* Enable the alarm irq */
	retval = ioctl(fd, RTC_WKALM_SET, wkalrm);
	if (retval >= 0) {
		printf("Waiting %d seconds for alarm... ",RTC_ALARM_SECS);
		fflush(stdout);

		FD_ZERO(&readfds);
		FD_SET(fd, &readfds);

		/* The select will wait until an RTC interrupt happens. */
		retval = select(fd + 1, &readfds, NULL, NULL, &tv);
		if (retval < 0) {
			perror("select");
			exit(EXIT_FAILURE);
		} else if (retval == 0) {
			/* Timeout */
			printf(" Timeout!\n");
		} else {
			retval = read(fd, &data, sizeof(unsigned long));
			if (retval < 0) {
				perror("read");
				exit(EXIT_FAILURE);
			}
			printf(" RING, RING, RING\n");
			result = 1;
		}
		/* Disable the alarm */
		wkalrm->enabled = 0;
		retval = ioctl(fd, RTC_WKALM_SET, wkalrm);
		if (retval < 0)
			result = 0;
	}

	return result;
}

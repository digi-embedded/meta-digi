/*
 * watchdog_test.c
 *
 * Copyright (C) 2009-2013 by Digi International Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * Description: Watchdog test application
 *
 */

#include <fcntl.h>
#include <linux/types.h>
#include <linux/watchdog.h>	/* WDIOC_SETTIMEOUT */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define	PROGRAM			"wd_test"
#define VERSION			"2.0"

#define	WD_DEVICE_FILE		"/dev/watchdog"

#define wd_test_usage \
	"[-t timeout (2)] [-d dead | disable (dead)] [-s write | ioctl (write)] [-n test_time (60)]\n"
#define wd_test_full_usage \
	"wd_test [options]\n\n" \
	"Tests the hardware watchdog\n" \
	"Options:\n" \
	"  -t : timeout in seconds (default 2)\n" \
	"  -d : dead | disable, action when the test finishs (default dead)\n" \
	"  -s : write | ioctl, system call used to kick the watchdog (default write)\n" \
	"  -n : test duration in seconds (default 60)\n" \
	"  -h : help\n\n"

/*
 * Function:    wd_test_banner
 * Description: print message
 */
static void wd_test_banner(void)
{
	fprintf(stdout, "%s %s Copyright Digi International Inc.\n\n"
		"Watchdog test/demo application\n\n", PROGRAM, VERSION);
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
		wd_test_banner();
		fprintf(stdout, "%s", wd_test_full_usage);
	} else {
		fprintf(stdout, "%s", wd_test_usage);
	}

	exit_error(NULL, exit_val);
}

/*
 * Function:    wd_keep_alive
 * Description: kick the watchdog using write or ioctl depending on the parameter
 */
static void wd_keep_alive(int fd, int system_call)
{
	int dummy;

	if (system_call) {
		if (ioctl(fd, WDIOC_KEEPALIVE, &dummy) < 0) {
			perror("ioctl");
			printf("Unable to kick the watchdog.\n");
		}
	} else {
		write(fd, "\0", 1);
	}
}

/*
 * Function:    main
 * Description: application's main function
 */
int main(int argc, char *argv[])
{
	int opt;
	int wd_timeout = 2;
	int wd_disable = 0;
	int wd_syscall = 0;
	int wd_test_time = 60;
	int wd_fd;

	if (argc > 1) {
		while ((opt = getopt(argc, argv, "t:d:s:n:h")) > 0) {
			switch (opt) {
			case 't':
				wd_timeout = atoi(optarg);
				break;

			case 'n':
				wd_test_time = atoi(optarg);
				break;

			case 'd':
				if (!strcmp(optarg, "dead")) {
					wd_disable = 0;
				} else if (!strcmp(optarg, "disable")) {
					wd_disable = 1;
				} else {
					show_usage_exit(EXIT_FAILURE, 0);
				}
				break;

			case 's':
				if (!strcmp(optarg, "write")) {
					wd_syscall = 0;
				} else if (!strcmp(optarg, "ioctl")) {
					wd_syscall = 1;
				} else {
					show_usage_exit(EXIT_FAILURE, 0);
				}
				break;
			case 'h':
			default:
				show_usage_exit((opt == 'h') ? EXIT_SUCCESS : EXIT_FAILURE, 1);
			}
		}
	}

	wd_test_banner();

	/* Open watchdog, this will start the watchdog counters */
	wd_fd = open(WD_DEVICE_FILE, O_RDWR);
	if (wd_fd < 0) {
		perror(WD_DEVICE_FILE);
		exit(EXIT_FAILURE);
	}
	printf("Watchdog (%s) opened and started\n", WD_DEVICE_FILE);

	if (ioctl(wd_fd, WDIOC_SETTIMEOUT, &wd_timeout) < 0) {
		perror("ioctl");
		close(wd_fd);
		show_usage_exit(EXIT_FAILURE, 0);
	}
	printf(" -Setting watchdog timeout to %d (sec)\n", wd_timeout);
	printf(" -Test will finish with %s\n",
	       wd_disable ? "watchdog disabled" : "board reset");

	printf("\n");

	while (wd_test_time) {
		wd_keep_alive(wd_fd, wd_syscall);
		sleep(1);
		wd_test_time--;
		printf("\r  Remaining time: %d   ", wd_test_time);
		fflush(stdout);
	}
	/* Kick it for the last time, so that the last sleep() does
	 * not affect the programmed reset time.
	 */
	wd_keep_alive(wd_fd, wd_syscall);
	printf("\n\n");

	if (wd_disable) {
		wd_timeout = WDIOS_DISABLECARD;
		if (ioctl(wd_fd, WDIOC_SETOPTIONS, &wd_timeout) < 0) {
			perror("ioctl");
			printf("Unable to disable watchdog. System will reset\n");
		}
		else {
			printf("Disabling watchdog\n");
		}
	} else {
		printf("Sayonara baby...");
	}

	printf("\n");
	fflush(stdout);
	close(wd_fd);

	return EXIT_SUCCESS;
}

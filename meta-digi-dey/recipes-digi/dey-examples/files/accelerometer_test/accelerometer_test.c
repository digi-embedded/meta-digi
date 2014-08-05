/*
 * accelerometer_test.c
 *
 * Copyright (C) 2012-2013 by Digi International Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * Description: On-Board accelerometer test application.
 *
 */

#include <dirent.h>
#include <fcntl.h>
#include <linux/input.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define INPUT_DEV_PATH		"/dev/input/"
#define DEFAULT_DEV_NAME	"mma7455l"
#define MILLISECOND		1000

static int fd;
static DIR *dir;

void printUsage(FILE * fd)
{
	fprintf(fd, "Usage: accelerometer_test -d device_name -h\n"
		"  Accelerometer test application:\n\n"
		"     -d Device name (default \"mma7455l\")\n"
		"     -t Timeout in seconds (default 60, 0 for no timeout)"
		"     -h This help\n\n");
}

void signal_handler(int sig)
{
	switch (sig) {
	case SIGHUP:
		printf("HANGUP signal catched\n");
		break;
	case SIGTERM:
		printf("TERMINATE signal catched\n");
		exit(EXIT_SUCCESS);
		break;
	case SIGINT:
		printf("INTERRUPT signal catched\n");
		exit(EXIT_SUCCESS);
		break;
	default:
		printf("Signal %d catched\n", sig);
		break;
	}
}

int main(int argc, char *argv[])
{
	int retval;
	fd_set fd_set_read;
	struct dirent *dir_entry;
	struct timeval timeout;
	int timeout_val = 60;
	int bytes_read;
	char filepath[64];
	int abs_x, abs_y, abs_z;
	int valid_x = 0, valid_y = 0, valid_z = 0;
	int opt;
	char dev_name[] = DEFAULT_DEV_NAME;
	struct input_event inp_event;
	char event_name[64];
	int event_found = 0;

	signal(SIGHUP, signal_handler);
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	/* Process arguments */
	while ((opt = getopt(argc, argv, "d:m:t:h")) != -1) {
		switch (opt) {
		case '?':
			printUsage(stderr);
			return 1;
		case 'h':
			printUsage(stdout);
			return 0;
		case 'd':
			strcpy(dev_name, optarg);
			break;
		case 't':
			timeout_val = atoi(optarg);
			break;
		default:
			printUsage(stdout);
			return 0;
		}
	}

	printf("Running accelerometer_test on device %s\nMode: Input event\n", dev_name);
	if (timeout_val)
		printf("Timeout: %d seconds\n", timeout_val);

	if ((dir = opendir(INPUT_DEV_PATH)) == NULL) {
		printf("Unable to open dir %s\n Exiting\n", INPUT_DEV_PATH);
		exit(EXIT_FAILURE);
	}

	/* Looking for directory entries which name starts by "event" */
	while ((dir_entry = readdir(dir))) {
		if ((dir_entry->d_type == DT_CHR) && !(memcmp(dir_entry->d_name, "event", 5))) {
			strcpy(filepath, INPUT_DEV_PATH);
			strcat(filepath, dir_entry->d_name);

			fd = open(filepath, O_RDONLY | O_NONBLOCK);
			if (fd < 0)
				continue;

			if (ioctl(fd, EVIOCGNAME(sizeof(event_name)), event_name) < 0) {
				printf("Unable to read %s's name", filepath);
			} else if (strcmp(event_name, dev_name) == 0) {
				printf("Accelerometer %s found\n", dev_name);
				event_found = 1;
				break;
			}
			close(fd);
		}
	}
	(void)closedir(dir);

	if (!event_found) {
		printf("Accelerometer %s not present\n", dev_name);
		exit(EXIT_FAILURE);
	}

	/* Configuring file descriptor set to poll input event */
	FD_ZERO(&fd_set_read);
	FD_SET(fd, &fd_set_read);

	printf("Waiting for valid data on 3 axis (move the module a little)\n");
	while (1) {
		timeout.tv_sec = timeout_val;
		retval =
		    select(FD_SETSIZE, &fd_set_read, NULL, NULL,
			   (timeout_val ? &timeout : NULL));
		/* Wait until fd has valid data */
		if (retval == 0) {
			perror("Timed out!\n");
			exit(EXIT_FAILURE);
		} else if (retval < 0) {
			perror("select()\n");
			exit(EXIT_FAILURE);
		}
		/* Read from input device until it doesn't return data */
		do {
			bytes_read = read(fd, &inp_event, sizeof(inp_event));

			if (bytes_read < 0) {
				break;	/* Resource temporarily not available */
			}
			if (bytes_read != sizeof(inp_event)) {
				printf("An error occurred when reading event: bytes read %d\n",
				       bytes_read);
				perror("ERROR");
				break;	/* wait for next select */
			}

			if (inp_event.type == EV_ABS) {
				/* Update axis' absolute values */
				switch (inp_event.code) {
				case ABS_X:
					abs_x = inp_event.value;
					valid_x = 1;
					break;
				case ABS_Y:
					abs_y = inp_event.value;
					valid_y = 1;
					break;
				case ABS_Z:
					abs_z = inp_event.value;
					valid_z = 1;
					break;
				}
			} else if (inp_event.type == EV_SYN && valid_x && valid_y && valid_z) {
				/* Only need to validate data the 1st time, so it's not necessary to reset flags */
				printf("X = %d	Y = %d	Z = %d\n", abs_x, abs_y, abs_z);
			}
		} while (bytes_read);
	}
}

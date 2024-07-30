/*
 * ConnectCore 6UL tamper sample application.
 *
 * Copyright (C) 2016, Digi International Inc.
 * All rights reserved.
 *
 * Based on iio_event_monitor.c from the tools/iio directory, of the linux
 * kernel.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 */

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/iio/events.h>
#include <linux/iio/types.h>
#include <sys/ioctl.h>

#include "iio_utils.h"

#define TAMPER_SAMPLE_USAGE				\
	"Usage:\n"					\
	"tamper_sample -i tamper_iface\n\n"

#define TAMPER_SAMPLE_FULL_USAGE			\
	"Usage:\n"					\
	"tamper_sample [options]\n\n"			\
	"Options:\n"					\
	"  -i : tamper interface to be used\n"		\
	"  -h : help\n\n"

typedef struct mca_tamper {
	struct iio_event_data event;
	char *dev_name;
	char *chrdev_name;
	int iface;
	int dev_num;
	int event_fd;
} mca_tamper_t;

mca_tamper_t tamper_data;

static void show_usage(int full)
{
	fprintf(stdout, "%s", full ?
		TAMPER_SAMPLE_FULL_USAGE : TAMPER_SAMPLE_USAGE);
}

static bool event_is_tamper(struct iio_event_data *event)
{
	enum iio_chan_type type = IIO_EVENT_CODE_EXTRACT_CHAN_TYPE(event->id);
	enum iio_event_type ev_type = IIO_EVENT_CODE_EXTRACT_TYPE(event->id);
	enum iio_event_direction dir = IIO_EVENT_CODE_EXTRACT_DIR(event->id);
	bool ret = true;

	if (type != IIO_ACTIVITY)
		ret = false;
	else if (ev_type != IIO_EV_TYPE_CHANGE)
		ret = false;
	else if (dir != IIO_EV_DIR_NONE)
		ret = false;

	return ret;
}

static void tamper_event_log(mca_tamper_t *tdata)
{
	/* Log the event in the system log, if any */
	fprintf(stdout, "tamper%d event! time: %lld\n",
		tdata->iface, tdata->event.timestamp);
}

static void tamper_event_actions(mca_tamper_t *tdata)
{
	/* Take the necessary defensive actions after a tamper event */
	fprintf(stdout, "tamper%d: taking actions!\n", tdata->iface);
}

static void tamper_event_ack(mca_tamper_t *tdata)
{
	int ret;
	char tamper_sysfs_dir[sizeof("/sys/bus/iio/devices/iio:deviceX")];

	ret = sprintf(tamper_sysfs_dir,
		      "/sys/bus/iio/devices/iio:device%d", tdata->dev_num);
	if (ret < 0) {
		fprintf(stdout, "Failed to build event ack file name\n");
		return;
	}

	/* Finally, acknowledge the event */
	ret = write_sysfs_string("tamper_events", tamper_sysfs_dir, "ack");
	if (ret < 0)
		fprintf(stdout, "Failed to acknowledge tamper%d event\n",
			tdata->iface);
}

static void process_tamper_event(mca_tamper_t *tdata, bool check_event)
{
	if (check_event && !event_is_tamper(&tdata->event)) {
		fprintf(stdout, "Unknown event: time: %lld, id: %llx\n",
			tdata->event.timestamp, tdata->event.id);
		return;
	}

	tamper_event_log(tdata);
	tamper_event_actions(tdata);
	tamper_event_ack(tdata);
}

static int check_tamper_event(mca_tamper_t *tdata)
{
	int ret;
	char tamper_sysfs_dir[sizeof("/sys/bus/iio/devices/iio:deviceX")];
	char str[50];

	ret = sprintf(tamper_sysfs_dir,
		      "/sys/bus/iio/devices/iio:device%d", tdata->dev_num);
	if (ret < 0) {
		fprintf(stdout, "Failed to build event file name\n");
		return -1;
	}

	ret = read_sysfs_string("tamper_events", tamper_sysfs_dir, str);
	if (ret < 0) {
		fprintf(stdout, "Failed to read tamper%d event\n",
			tdata->iface);
		return -1;
	}

	if (!strncmp(str, "none", strlen("none"))) {
		/* Continue to wait for tamper event to happen */
		return 0;
	} else if (!strncmp(str, "signaled+acked", strlen("signaled+acked"))) {
		fprintf(stdout, "Tamper event already acknowledged, not taking actions\n");
		return 1;
	} else if (!strncmp(str, "signaled", strlen("signaled"))) {
		tdata->event.timestamp = read_sysfs_posint("timestamp",
							   tamper_sysfs_dir);
		if (tdata->event.timestamp < 0)
			fprintf(stdout, "Failed to read timestamp for tamper%d!\n",
				tdata->iface);

		fprintf(stdout, "Tamper event was already signaled.\n");
		process_tamper_event(tdata, false);
		return 1;
	}

	fprintf(stdout, "Unkown status for tamper%d: '%s'\n", tdata->iface, str);
	return -1;
}

int main(int argc, char **argv)
{
	mca_tamper_t *tdata = &tamper_data;
	int ret;
	int opt;
	int fd;

	memset(tdata, 0, sizeof(mca_tamper_t));

	if (argc <= 1) {
		show_usage(0);
		return EXIT_FAILURE;
	}

	while ((opt = getopt(argc, argv, "i:h")) > 0) {
		switch (opt) {
		case 'i':
			tdata->iface = atoi(optarg);
			break;

		case 'h':
			show_usage(1);
			return EXIT_SUCCESS;

		default:
			show_usage(0);
			return EXIT_FAILURE;
		}
	}

	ret = asprintf(&tdata->dev_name, "TAMPER%d", tdata->iface);
	if (ret < 0) {
		fprintf(stdout, "Failed to find interface, device name too long?\n");
		ret = -ENOMEM;
		goto error_ret;
	}

	tdata->dev_num = find_type_by_name(tdata->dev_name, "iio:device");
	if (tdata->dev_num < 0) {
		fprintf(stdout, "Failed to find iio:device for TAMPER%d\n",
			tdata->iface);
		ret = -ENODEV;
		goto error_ret2;
	}

	ret = asprintf(&tdata->chrdev_name, "/dev/iio:device%d", tdata->dev_num);
	if (ret < 0) {
		fprintf(stdout, "Failed to allocate memory\n");
		ret = -ENOMEM;
		goto error_ret2;
	}

	ret = check_tamper_event(tdata);
	if (ret)
		goto error_ret3;

	fd = open(tdata->chrdev_name, 0);
	if (fd < 0) {
		fprintf(stdout, "Failed to open %s\n", tdata->chrdev_name);
		ret = -errno;
		goto error_ret3;
	}

	ret = ioctl(fd, IIO_GET_EVENT_FD_IOCTL, &tdata->event_fd);

	close(fd);

	if (ret < 0 || tdata->event_fd < 0) {
		fprintf(stdout, "Failed to retrieve event fd\n");
		ret = -errno;
		goto error_ret3;
	}

	fprintf(stdout, "Waiting for tamper events:\n");

	while (true) {
		ret = read(tdata->event_fd, &tdata->event, sizeof(struct iio_event_data));
		if (ret < 0) {
			if (errno == EAGAIN) {
				fprintf(stdout, "No events... continue\n");
				continue;
			} else {
				perror("Failed to read event from device");
				ret = -errno;
				break;
			}
		}

		process_tamper_event(tdata, true);
	}

	close(tdata->event_fd);

error_ret3:
	free(tdata->chrdev_name);
error_ret2:
	free(tdata->dev_name);
error_ret:
	return ret;
}

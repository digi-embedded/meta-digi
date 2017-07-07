/*
 * ConnectCore 6UL Analog Comparator sample application.
 *
 * Copyright (c) 2017 Digi International Inc.
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

#define BUFFER_LEN		20

#define FULL_USAGE							\
	"Usage:\n"							\
	"adc_cmp_sample -c channel [options]\n\n"			\
	"Options:\n"							\
	"  -c : channel number to read from\n"				\
	"  -h : Threshold_high higher limit of the comparator window\n"	\
	"  -l : Threshold_low lower limit of the comparator window\n"	\
	"  -e : edges enabled: 'rising', 'falling' or 'both'\n"	\
	"  -v : use V for output and thresholds instead of raw values.\n"\
	"  -? : help\n\n"

typedef struct cmp {
	char *sysfs_dir;
	unsigned int channel;
	double voltage_scale;
	bool raw;
} cmp_t;

static void show_usage()
{
	fprintf(stdout, "%s", FULL_USAGE);
}

static int read_adc_sample_sysfs(cmp_t *cmp, long int *val)
{
	int fd = -1;
	int ret;
	char buffer[BUFFER_LEN];
	char *path = NULL;

	ret = asprintf(&path, "%s/in_voltage%u_raw",
		       cmp->sysfs_dir, cmp->channel);
	if (ret < 0) {
		fprintf(stdout, "Failed to allocate memory\n");
		ret = -ENOMEM;
		goto exit;
	}

	fd = open(path, O_RDONLY);
	if (fd < 0) {
		fprintf(stdout, "%s: failed to open %s\n",
			__func__, path);
		ret = fd;
		goto exit;
	}

	ret = read(fd, buffer, BUFFER_LEN);
	if (ret < 0) {
		fprintf(stdout, "%s: failed to read ADC sample from %s (%d)\n",
			__func__, path, ret);
		goto exit;
	}

	if (ret == 0) {
		fprintf(stdout, "%s: no data available in %s\n",
			__func__, path);
		ret = -ENODATA;
		goto exit;
	}

	*val = strtol(buffer, NULL, 10);
	ret = 0;

exit:
	free(path);
	close(fd);
	return ret;
}

static int read_cmp_out_sysfs(cmp_t *cmp, int *val)
{
	int fd = -1;
	int ret;
	char buffer[BUFFER_LEN];
	char *path = NULL;

	ret = asprintf(&path, "%s/in_voltage%u_cmp_out",
		       cmp->sysfs_dir, cmp->channel);
	if (ret < 0) {
		fprintf(stdout, "Failed to allocate memory\n");
		ret = -ENOMEM;
		goto exit;
	}

	fd = open(path, O_RDONLY);
	if (fd < 0) {
		fprintf(stdout, "%s: failed to open %s\n",
			__func__, path);
		ret = fd;
		goto exit;
	}

	ret = read(fd, buffer, BUFFER_LEN);
	if (ret < 0) {
		fprintf(stdout,
			"%s: failed to read Comparator Output from %s (%d)\n",
			__func__, path, ret);
		goto exit;
	} else if (ret == 0) {
		fprintf(stdout, "%s: no data available in %s\n",
			__func__, path);
		ret = -ENODATA;
		goto exit;
	}

	*val = strtol(buffer, NULL, 10);
	ret = 0;

exit:
	free(path);
	close(fd);

	return ret;
}

static void process_event(cmp_t *cmp, struct iio_event_data *event)
{
	enum iio_event_direction dir = IIO_EVENT_CODE_EXTRACT_DIR(event->id);
	int channel = IIO_EVENT_CODE_EXTRACT_CHAN(event->id);
	const char *dir_str;
	long sample_val;
	int cmp_out;
	int ret;

	switch (dir) {
	case IIO_EV_DIR_EITHER:
		dir_str = "Both";
		break;
	case IIO_EV_DIR_RISING:
		dir_str = "Rising";
		break;
	case IIO_EV_DIR_FALLING:
		dir_str = "Falling";
		break;
	default:
		dir_str = "Unknown";
		break;
	}

	ret = read_adc_sample_sysfs(cmp, &sample_val);
	if (ret) {
		fprintf(stdout, "Failed to read ADC value\n");
		return;
	}

	ret = read_cmp_out_sysfs(cmp, &cmp_out);
	if (ret) {
		fprintf(stdout, "Failed to read CMP Out\n");
		return;
	}

	fprintf(stdout, "\n\nGot a Comparator event!\n");
	fprintf(stdout, "\tTime: %lld\n", event->timestamp);
	fprintf(stdout, "\tChannel: %d\n", channel);
	fprintf(stdout, "\tCMP Out: %d\n", (unsigned int)cmp_out);
	fprintf(stdout, "\tEdge: %s\n", dir_str);

	if (cmp->raw)
		fprintf(stdout, "\tADC Value: 0x%04x\n",
			(unsigned int)sample_val);
	else
		fprintf(stdout, "\tADC Value: %.2f V\n",
			sample_val * cmp->voltage_scale / 1000);
}

static int read_voltage_scale(cmp_t *cmp)
{
	int fd, ret;
	char buffer[BUFFER_LEN];
	char *temp;

	if (cmp->raw)
		return 0;

	/* Read the voltage scale from the sysfs */
	ret = asprintf(&temp, "%s/in_voltage_scale", cmp->sysfs_dir);
	if (ret < 0) {
		fprintf(stdout, "%s: failed to allocate memory\n", __func__);
		return -ENOMEM;
	}

	fd = open(temp, O_RDONLY);
	if (fd < 0) {
		fprintf(stdout, "%s: failed to open %s\n", __func__, temp);
		ret = fd;
		goto free_temp;
	}

	ret = read(fd, buffer, BUFFER_LEN);
	if (ret < 0) {
		fprintf(stdout,
			"%s: failed to read voltage scale from %s (%d)\n",
			__func__, temp, ret);
		goto close_fd;
	} else if (ret == 0) {
		fprintf(stdout,
			"%s: no data available in %s\n", __func__, temp);
		ret = -ENODATA;
		goto close_fd;
	}

	cmp->voltage_scale = atof(buffer);

close_fd:
	close(fd);
free_temp:
	free(temp);
	return ret;
}

static int write_in_path(const char *path, const char *str)
{
	int fd = -1;
	int ret;

	fd = open(path, O_RDWR);
	if (fd < 0) {
		fprintf(stdout, "%s: failed to open %s\n",
			__func__, path);
		ret = fd;
		goto exit;
	}

	ret = write(fd, str, strlen(str));
	if (ret < 0) {
		fprintf(stdout, "%s: failed to write data to %s (%d)\n",
			__func__, path, ret);
		goto exit;
	}

exit:
	close(fd);
	return ret;
}

static int configure_comparator(cmp_t *cmp, uint16_t th_l, uint16_t th_h,
				char *edge)
{
	char *path;
	char buffer[BUFFER_LEN];
	int ret;

	/* Allocate a few bytes more, since it's reused several times */
	path = malloc(strlen(cmp->sysfs_dir) + strlen("/in_voltageX_cmp_thr_l")
		      + 15);
	if (!path) {
		fprintf(stdout, "Failed to allocate memory\n");
		goto exit;
	}

	/* Configure Threshold LOW */
	sprintf(path, "%s/in_voltage%u_cmp_thr_l",
		cmp->sysfs_dir, cmp->channel);
	sprintf(buffer, "%d", th_l);

	ret = write_in_path(path, buffer);
	if (ret < 0)
		goto exit;

	/* Configure Threshold HIGH */
	sprintf(path, "%s/in_voltage%u_cmp_thr_h",
		cmp->sysfs_dir, cmp->channel);
	sprintf(buffer, "%d", th_h);

	ret = write_in_path(path, buffer);
	if (ret < 0)
		goto exit;

	ret = write_in_path(path, buffer);
	if (ret < 0)
		goto exit;

	/* Configure Edge */
	sprintf(path, "%s/in_voltage%u_cmp_edge",
		cmp->sysfs_dir, cmp->channel);

	ret = write_in_path(path, edge);
	if (ret < 0)
		goto exit;
exit:
	free(path);

	return ret;
}

int main(int argc, char **argv)
{
	int ret, opt;
	int fd = -1;
	int event_fd = -1;
	char *chrdev_path = NULL;
	char *edge = NULL;
	int dev_num;
	double threshold_low = 0;
	double threshold_high = 0xFFFF;
	cmp_t cmp = {
		.sysfs_dir = NULL,
		.raw = true,
	};
	struct iio_event_data event;

	if (argc <= 3) {
		show_usage();
		return EXIT_FAILURE;
	}

	while ((opt = getopt(argc, argv, "c:h:l:e:v?")) > 0) {
		switch (opt) {
		case 'c':
			cmp.channel = strtoul(optarg, NULL, 10);
			break;
		case 'h':
			threshold_high = atof(optarg);
			break;
		case 'l':
			threshold_low = atof(optarg);
			break;
		case 'e':
			if (!strcmp(optarg, "rising") &&
			    !strcmp(optarg, "falling") &&
			    !strcmp(optarg, "both")) {
				fprintf(stdout,
					"Invalid edge (%s)\n",
					optarg);
				goto exit;
			}
			edge = strdup(optarg);
			if (!edge) {
				fprintf(stdout, "Failed to allocate memory\n");
				goto exit;
			}
			break;
		case 'v':
			cmp.raw = false;
			break;
		case '?':
			show_usage();
			return EXIT_SUCCESS;
		default:
			show_usage();
			return EXIT_FAILURE;
		}
	}

	if (!edge) {
		edge = strdup("both");
		if (!edge) {
			fprintf(stdout, "Failed to allocate memory\n");
			goto exit;
		}
	}

	dev_num = find_type_by_name("mca-cc6ul-adc", "iio:device");
	if (dev_num < 0) {
		fprintf(stdout,
			"Failed to find iio:device for mca-cc6ul-adc\n");
		ret = -ENODEV;
		goto exit;
	}

	ret = asprintf(&chrdev_path, "/dev/iio:device%d", dev_num);
	if (ret < 0) {
		fprintf(stdout, "Failed to allocate memory\n");
		ret = -ENOMEM;
		goto exit;
	}

	ret = asprintf(&cmp.sysfs_dir, "/sys/bus/iio/devices/iio:device%d",
		       dev_num);
	if (ret < 0) {
		fprintf(stdout, "Failed to allocate memory\n");
		ret = -ENOMEM;
		goto exit;
	}

	if (!cmp.raw) {
		ret = read_voltage_scale(&cmp);
		if (ret < 0)
			goto exit;
		threshold_high = threshold_high * 1000 / cmp.voltage_scale;
		threshold_low = threshold_low * 1000 / cmp.voltage_scale;
	}

	ret = configure_comparator(&cmp, threshold_low, threshold_high, edge);
	if (ret < 0)
		goto exit;


	fd = open(chrdev_path, 0);
	if (fd < 0) {
		fprintf(stdout, "Failed to open %s\n", chrdev_path);
		ret = fd;
		goto exit;
	}

	ret = ioctl(fd, IIO_GET_EVENT_FD_IOCTL, &event_fd);

	close(fd);
	if (ret < 0 || event_fd < 0) {
		fprintf(stdout, "Failed to retrieve event fd\n");
		ret = fd;
		goto exit;
	}

	fprintf(stdout, "Waiting for events:\n");

	while (true) {
		ret = read(event_fd, &event, sizeof(event));
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
		process_event(&cmp, &event);
	}

exit:
	free(cmp.sysfs_dir);
	free(chrdev_path);
	free(edge);
	close(event_fd);

	return ret;
}

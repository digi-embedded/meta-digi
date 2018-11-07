/*
 * ConnectCore 6UL ADC sample application.
 *
 * Copyright (c) 2016 Digi International Inc.
 * All rights reserved.
 *
 * Partially based on iio_event_monitor.c from the tools/iio directory, of the
 * linux kernel.
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
#include <poll.h>
#include <sys/ioctl.h>

#include "iio_utils.h"

#define ARRAY_SIZE(v)		(sizeof(v) / sizeof((v)[0]))
#define BUFFER_LEN		20

#define ADC_SAMPLE_USAGE						\
	"Usage:\n"							\
	"adc_sample -t ADC_type -c channel [options]\n\n"

#define ADC_SAMPLE_FULL_USAGE						\
	"Usage:\n"							\
	"adc_sample -t ADC_type -c channel [options]\n\n"		\
	"Options:\n"							\
	"  -t : ADC_type ('MX6UL', 'MCA-CC6UL', 'MCA-CC8X', 'IOEXP')\n"	\
	"  -c : channel number to read from\n"				\
	"  -n : Number of samples (default: 1)\n"			\
	"  -d : Delay (in ms) between samples (default: 1000)\n"	\
	"  -v : Show output in V (otherwise raw ADC value is shown)\n"	\
	"  -h : help\n\n"

enum adc_type {
	ADC_TYPE_UNKNOWN,
	ADC_TYPE_MX6UL,
	ADC_TYPE_MCA_CC6UL,
	ADC_TYPE_MCA_CC8X,
	ADC_TYPE_IOEXP,
};

struct adc_data {
	enum adc_type type;
	const char *name;
	const char *dev_name;
	unsigned int nbits;
};

struct adc_data adc_list[] = {
	{
		.type 		= ADC_TYPE_MX6UL,
		.name 		= "MX6UL",
		.dev_name 	= "2198000.adc",
		.nbits 		= 12,
	},
	{
		.type 		= ADC_TYPE_MCA_CC6UL,
		.name 		= "MCA-CC6UL",
		.dev_name 	= "mca-cc6ul-adc",
		.nbits 		= 12,
	},
	{
		.type 		= ADC_TYPE_MCA_CC8X,
		.name 		= "MCA-CC8X",
		.dev_name 	= "mca-cc8x-adc",
		.nbits 		= 12,
	},
	{
		.type 		= ADC_TYPE_IOEXP,
		.name 		= "IOEXP",
		.dev_name 	= "mca-ioexp-adc",
		.nbits 		= 12,
	},
};

typedef struct adc {
	struct adc_data *data;
	char *chrdev_name;
	char *sysfs_file;
	char *sysfs_dir;
	int dev_num;
	unsigned long channel;
	double voltage_scale;
} adc_t;

static void show_usage(int full)
{
	fprintf(stdout, "%s", full ?
		ADC_SAMPLE_FULL_USAGE : ADC_SAMPLE_USAGE);
}

static struct adc_data *get_adc_data(const char *type_str)
{
	struct adc_data *data = NULL;
	int i;

	for (i = 0; i < ARRAY_SIZE(adc_list); i++) {
		if (!strcmp(adc_list[i].name, type_str)) {
			data = &adc_list[i];
			break;
		}
	}

	return data;
}

static int read_adc_sample_sysfs(adc_t *adc, long int *val)
{
	int fd, ret;
	char buffer[BUFFER_LEN];

	fd = open(adc->sysfs_file, O_RDONLY);
	if (fd < 0) {
		fprintf(stdout, "%s: failed to open %s\n",
			__func__, adc->sysfs_file);
		ret = fd;
		goto just_ret;
	}

	ret = read(fd, buffer, BUFFER_LEN);
	if (ret < 0) {
		fprintf(stdout, "%s: failed to read ADC sample from %s (%d)\n",
			__func__, adc->sysfs_file, ret);
		goto close_ret;
	}

	if (ret == 0) {
		fprintf(stdout, "%s: no data available in %s\n",
			__func__, adc->sysfs_file);
		ret = -ENODATA;
		goto close_ret;
	}

	*val = strtol(buffer, NULL, 10);
	ret = 0;

close_ret:
	close(fd);
just_ret:
	return ret;
}

static int read_voltage_scale(adc_t *adc, double *val)
{
	int fd, ret;
	char buffer[BUFFER_LEN];
	char *temp;

	/* Read the voltage scale from the sysfs */
	ret = asprintf(&temp, "%s/in_voltage_scale", adc->sysfs_dir);
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
		fprintf(stdout, "%s: failed to voltage scale from %s (%d)\n",
			__func__, temp, ret);
		goto close_fd;
	}

	if (ret == 0) {
		fprintf(stdout, "%s: no data available in %s\n", __func__, temp);
		ret = -ENODATA;
		goto close_fd;
	}

	*val = atof(buffer);

close_fd:
	close(fd);
free_temp:
	free(temp);
	return ret;
}

int main(int argc, char **argv)
{
	adc_t *adc;
	unsigned long nsamples = 1;
	unsigned long delay_ms = 1000;
	long sample_val;
	bool raw = true;
	int ret, opt, i;

	if (argc <= 2) {
		show_usage(1);
		return EXIT_FAILURE;
	}

	adc = malloc(sizeof(adc_t));
	if (!adc) {
		fprintf(stdout, "Failed to allocate memory\n");
		ret = -ENOMEM;
		goto error_ret;
	}
	memset(adc, 0, sizeof(adc_t));

	adc->channel = ~0;

	while ((opt = getopt(argc, argv, "t:n:d:c:vh")) > 0) {
		switch (opt) {
		case 't':
			adc->data = get_adc_data(optarg);
			if (!adc->data) {
				fprintf(stdout, "Unknown ADC type %s\n", optarg);
				show_usage(0);
				ret = EXIT_FAILURE;
				goto error_ret2;
			}
			break;

		case 'n':
			nsamples = strtoul(optarg, NULL, 10);
			if (!nsamples) {
				fprintf(stdout,
					"Invalid number of samples parameter (%s)\n",
					optarg);
				show_usage(0);
				ret = EXIT_FAILURE;
				goto error_ret2;
			}
			break;

		case 'd':
			delay_ms = strtoul(optarg, NULL, 10);
			if (!delay_ms) {
				fprintf(stdout,
					"Invalid inter sample delay parameter (%s)\n",
					optarg);
				show_usage(0);
				ret = EXIT_FAILURE;
				goto error_ret2;
			}
			break;

		case 'c':
			adc->channel = strtoul(optarg, NULL, 10);
			break;

		case 'v':
			raw = false;
			break;

		case 'h':
			show_usage(1);
			return EXIT_SUCCESS;

		default:
			show_usage(0);
			return EXIT_FAILURE;
		}
	}

	/* Check that the application params provide what we need */
	if (!adc->data || adc->data->type == ADC_TYPE_UNKNOWN) {
		fprintf(stdout, "ADC type must be provided\n");
		show_usage(1);
		ret = EXIT_FAILURE;
		goto error_ret2;
	}

	if (adc->channel == ~0) {
		fprintf(stdout, "ADC channel must be provided\n");
		show_usage(1);
		ret = EXIT_FAILURE;
		goto error_ret2;
	}

	adc->dev_num = find_type_by_name(adc->data->dev_name, "iio:device");
	if (adc->dev_num < 0) {
		fprintf(stdout, "Failed to find iio:device for %s\n",
			adc->data->dev_name);
		ret = -ENODEV;
		goto error_ret2;
	}
	ret = asprintf(&adc->chrdev_name, "/dev/iio:device%d", adc->dev_num);
	if (ret < 0) {
		fprintf(stdout, "Failed to allocate memory\n");
		ret = -ENOMEM;
		goto error_ret2;
	}

	ret = asprintf(&adc->sysfs_dir, "/sys/bus/iio/devices/iio:device%d",
		       adc->dev_num);
	if (ret < 0) {
		fprintf(stdout, "Failed to allocate memory\n");
		ret = -ENOMEM;
		goto error_ret3;
	}

	ret = asprintf(&adc->sysfs_file, "%s/in_voltage%lu_raw",
		       adc->sysfs_dir, adc->channel);
	if (ret < 0) {
		fprintf(stdout, "Failed to allocate memory\n");
		ret = -ENOMEM;
		goto error_ret4;
	}

	if (!raw) {
		ret = read_voltage_scale(adc, &adc->voltage_scale);
		if (ret < 0) {
			goto error_ret5;
		}
	}

	for (i = 0; i < nsamples; i++) {
		ret = read_adc_sample_sysfs(adc, &sample_val);
		if (ret)
			break;

		if (raw) {
			fprintf(stdout, "Sample %i: 0x%04x\n",
				i, (unsigned int)sample_val);
		} else {
			fprintf(stdout, "Sample %i: %.2f V\n",
				i, sample_val * adc->voltage_scale / 1000);
		}

		usleep(delay_ms * 1000);
	}

error_ret5:
	free(adc->sysfs_file);
error_ret4:
	free(adc->sysfs_dir);
error_ret3:
	free(adc->chrdev_name);
error_ret2:
	free(adc);
error_ret:
	return ret;
}

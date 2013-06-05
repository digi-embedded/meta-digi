/*
 * adc_test.c
 *
 * Copyright (C) 2009-2013 by Digi International Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * Description: ADC test application
 *
 */
#include <fcntl.h>
#include <getopt.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if defined(CCIMX51JS) || defined(CCIMX53JS)
#include <linux/pmic_adc.h>
#include <sys/ioctl.h>
#elif defined(CCARDIMX28JS)
#include <linux/mxs-lradc.h>
#include <sys/ioctl.h>
#endif

#define	PROGRAM			"adc_test"
#define VERSION			"2.0"

#if defined(CCIMX53JS)
# define ADC_MAX_CHANNELS	10
#elif defined(CCIMX51JS)
# define ADC_MAX_CHANNELS	8
#endif

#if defined(CCIMX51JS) || defined(CCIMX53JS)
# define ADC_CONVERT_IOCTL	PMIC_ADC_CONVERT
# define ADC_CHARDEV		"/dev/pmic_adc"
#elif defined(CCARDIMX28JS)
# define ADC_MAX_CHANNELS	7
# define ADC_CONVERT_IOCTL	LRADC_CONVERT
# define ADC_CHARDEV		"/dev/mxs_lradc"
#endif

#define ADC_MAX_SAMPLE_BUFFER	10000

#define adc_test_usage \
	"[-i ms] [-n samples] [-v verbosity] -c chnum[,chnum]\n"
#define adc_test_full_usage \
	"adc_test [options] channel list\n\n" \
	"Tests the ADC channels\n" \
	"Options:\n" \
	"  -n  --numsamples numsamples          Number of samples to display (default 100)\n" \
	"  -v  --verbosity  level               Verbosity level (0-display only statistics, 1-display all samples)\n" \
	"  -c  --channels ch_num[,ch_num]       (comma separated without spaces)\n" \
	"  -i  --isms num_of_ms                 Intersample number of ms (default 100)\n" \
	"  -h  --help                           Usage information\n" \
	"      --version                        Show version\n"

typedef struct adc_channel {
	int fd;
	int channel;
	unsigned short int *samples;
	unsigned int samplecount;
	unsigned int avg;
	unsigned int variance;
	unsigned short maxval;
	unsigned short minval;
} adc_channel_t;

static char *chlist = NULL;

/*
 * Function:    adc_test_banner
 * Description: print banner
 */
static void adc_test_banner(void)
{
	fprintf(stdout, "%s %s Copyright 2010 Digi International Inc.\n"
		"ADC test/demo application\n\n", PROGRAM, VERSION);
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
		adc_test_banner();
		fprintf(stdout, "%s", adc_test_full_usage);
	} else {
		fprintf(stdout, "%s", adc_test_usage);
	}

	exit_error(NULL, exit_val);
}

/*
 * Function:    compute_ch_statistics
 * Description: compute some statistics over the stored samples
 */
static void compute_ch_statistics(adc_channel_t *adcch)
{
	int i;
	unsigned int avg = 0;
	unsigned int variance = 0;

	/* Compute the average */
	for (i = 0; i < adcch->samplecount; i++) {
		avg += adcch->samples[i];
		if (adcch->samples[i] > adcch->maxval)
			adcch->maxval = adcch->samples[i];
		if (adcch->samples[i] < adcch->minval)
			adcch->minval = adcch->samples[i];
	}
	/* compute average first so we can compute the variance */
	avg /= adcch->samplecount;

	/* compute the variance with a new loop to avoid rounding errors when
	 * computing the average  */
	for (i = 0; i < adcch->samplecount; i++)
		variance += ((adcch->samples[i] - avg) * (adcch->samples[i] - avg));

	adcch->variance = variance / adcch->samplecount;
	adcch->avg = avg;
}

/*
 * Function:    print_ch_statistics
 * Description: print channel statistic information
 */
static void print_ch_statistics(adc_channel_t *adcch)
{
	fprintf(stdout, "Channel %d Statistics (samples %d)\n", adcch->channel,
		adcch->samplecount);
	fprintf(stdout, "----------------------------------------------\n");
	fprintf(stdout, " Average       = %d\n", adcch->avg);
	fprintf(stdout, " Variance      = %d\n", adcch->variance);
	fprintf(stdout, " Std deviation = %d\n", (int)sqrt(adcch->variance));
	fprintf(stdout, " max val       = %d\n", adcch->maxval);
	fprintf(stdout, " min val       = %d\n\n", adcch->minval);
}

/*
 * Function:    main
 * Description: main function
 */
int main(int argc, char *argv[])
{
	adc_channel_t channel[ADC_MAX_CHANNELS];
	char device_name[30], *var;
	int ret, i, j, k, numsamples = 100;
	int verbose = 0;
	int intersample_us = 100000;
	static int version = 0;
	static int opt_index, opt, optcount = 0;
	static const char *short_options = "hi:c:n:v:";
	static const struct option long_options[] = {
		{"help",     no_argument,       NULL,    'h'},
		{"isms",     required_argument, NULL,    'i'},
		{"channels", required_argument, NULL,    'c'},
		{"samples",  required_argument, NULL,    'n'},
		{"verbose",  required_argument, NULL,    'v'},
		{"version",  no_argument,       &version, 1 },
		{0, 0, 0, 0},
	};

#if defined(CCIMX51JS) || defined(CCIMX53JS) || defined(CCARDIMX28JS)
    t_adc_convert_param adc_convert_param;
#else
    unsigned short int adcval;
#endif

	for (opt_index = 0;;) {

		opt = getopt_long(argc, argv, short_options, long_options, &opt_index);
		if (opt == EOF)
			break;

		switch (opt) {
		case 0:
			if (version) {
				printf("%s %s, compiled on %s, %s\n", PROGRAM, VERSION,
				       __DATE__, __TIME__);
				exit(EXIT_SUCCESS);
			}
			break;
		case 'n':
			numsamples = atoi(optarg);
			break;
		case 'i':
			intersample_us = atoi(optarg) * 1000;
			break;
		case 'c':
			chlist = optarg;
			break;
		case 'v':
			verbose = atoi(optarg);
			break;
		case 'h':
		case '?':
			show_usage_exit((opt == 'h') ? EXIT_SUCCESS : EXIT_FAILURE, 1);
			break;
		}
		optcount++;
	}

	if (optcount == 0)
		show_usage_exit(EXIT_FAILURE, 1);

	/* Check options */
	if (chlist == NULL)
		show_usage_exit(EXIT_FAILURE, 1);

	/* Initialize some variables */
	for (i = 0; i < ADC_MAX_CHANNELS; i++) {
		channel[i].fd = -1;
		channel[i].avg = 0;
		channel[i].samplecount = 0;
		channel[i].maxval = 0;
		channel[i].minval = 4095;
	}

	var = strtok(chlist, ",");
	while (var != NULL) {
		char *end;
		int chnr = strtol(var, &end, 10);
		if ((chnr >= 0) && (chnr < ADC_MAX_CHANNELS) && (var != end)) {
#if defined(CCIMX51JS) || defined(CCIMX53JS) || defined(CCARDIMX28JS)
			sprintf(device_name, ADC_CHARDEV);
#else
			snprintf(device_name, 30, "%s%d", ADC_CHARDEV, chnr);
#endif
			channel[chnr].channel = chnr;
			channel[chnr].fd = open(device_name, O_RDONLY);
			if (channel[chnr].fd < 0) {
				fprintf(stderr,
					"Unable to open adc channel %d (device file %s)\n",
					chnr, device_name);
			}
			channel[chnr].samples = malloc((numsamples > ADC_MAX_SAMPLE_BUFFER) ?
						       ADC_MAX_SAMPLE_BUFFER *
						       sizeof(unsigned short int) : numsamples *
						       sizeof(unsigned short int));
		}
		var = strtok(NULL, ",");
	}

	adc_test_banner();

	/* Check if any channel is available */
	int any_channel = 0;
	for (i = 0; i < ADC_MAX_CHANNELS; i++)
		any_channel |= (channel[i].fd >= 0);
	if (!any_channel)
		exit_error("No adc channel available (verify command line arguments)\n",
			   EXIT_FAILURE);

	for (j = 0; j < numsamples; j++) {
		k = j % ADC_MAX_SAMPLE_BUFFER;
		for (i = 0; i < ADC_MAX_CHANNELS; i++) {
			if (channel[i].fd >= 0) {
#if defined(CCIMX51JS) || defined(CCIMX53JS) || defined(CCARDIMX28JS)
				memset(&adc_convert_param,0,sizeof(adc_convert_param));
				adc_convert_param.channel = channel[i].channel;
#if defined(CCIMX51JS)
				/* The PMIC ADC driver maps GEN_PURPOSE_AD5,GEN_PURPOSE_AD6 and
				* GEN_PURPOSE_AD7 to channels 10,11,12*/
				if( adc_convert_param.channel > 4 )
					adc_convert_param.channel = adc_convert_param.channel + 5;
#endif
				fflush(stdout);
				if ((ret = ioctl(channel[i].fd, ADC_CONVERT_IOCTL, &adc_convert_param)) != 0) {
						fprintf(stderr,
							"Unable to read adc channel %d (device file %s)\n",
							i, device_name);
						continue;
				}
				channel[i].samples[k] = adc_convert_param.result[0];
				if (verbose > 0)
					fprintf(stdout, "CH %d (%03d): %04d [0x%3x]\n", i, k,
							adc_convert_param.result[0], adc_convert_param.result[0]);
#else
				ret = read(channel[i].fd, &adcval, sizeof(unsigned short int));
				if (ret < 0) {
					fprintf(stderr,
						"Unable to read adc channel %d (device file %s)\n",
						i, device_name);
					continue;
				}
				channel[i].samples[k] = adcval;
				if (verbose > 0)
					fprintf(stdout, "CH %d (%03d): %04d [0x%3x]\n", i, k,
						adcval, adcval);
#endif
				channel[i].samplecount++;

				if ((k + 1) == ADC_MAX_SAMPLE_BUFFER) {
					if (verbose > 0)
						fprintf(stdout, "\n");
					compute_ch_statistics(&channel[i]);
					print_ch_statistics(&channel[i]);
					channel[i].samplecount = 0;
				}
			}
		}
		usleep(intersample_us);
	}

	if (numsamples % ADC_MAX_SAMPLE_BUFFER) {
		if (verbose > 0)
			fprintf(stdout, "\n");
		for (i = 0; i < ADC_MAX_CHANNELS; i++) {
			if (channel[i].fd >= 0) {
				compute_ch_statistics(&channel[i]);
				print_ch_statistics(&channel[i]);
			}
		}
	}

	for (i = 0; i < ADC_MAX_CHANNELS; i++) {
		if (channel[i].fd >= 0) {
			close(channel[i].fd);
			free(channel[i].samples);
		}
	}

	return EXIT_SUCCESS;
}

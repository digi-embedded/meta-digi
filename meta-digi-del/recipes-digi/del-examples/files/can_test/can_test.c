/*
 * can_test.c
 *
 * Copyright (C) 2009-2013 by Digi International Inc.
 * All rights reserved.
 *
 * Description: CAN bus test application
 *
 * Based on canecho.c from socket-can project with following notice:
 *
 * Copyright (c) 2002-2007 Volkswagen Group Electronic Research
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of Volkswagen nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * Alternatively, provided that this notice is retained in full, this
 * software may be distributed under the terms of the GNU General
 * Public License ("GPL") version 2, in which case the provisions of the
 * GPL apply INSTEAD OF those given above.
 *
 * The provided data structures and external interfaces from this code
 * are not restricted to be used by modules with a GPL compatible license.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 * Send feedback to <socketcan-users@lists.berlios.de>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <libgen.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <net/if.h>
#include <getopt.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>

#include <linux/can.h>
#include <linux/can/raw.h>

/* Application infos */
#define APP_NAME			"can_test"
#define APP_VERSION			"2.0"

#define pr_error(...)			do { myprintf(ERROR, __VA_ARGS__); } while(0)
#define pr_warning(...)			do { myprintf(WARNING, __VA_ARGS__); } while(0)
#define pr_info(...)			do { myprintf(INFO, __VA_ARGS__); } while(0)
#define pr_debug(...)			do { myprintf(DEBUG, __VA_ARGS__); } while(0)
#define pr_naked(...)			do { printf(__VA_ARGS__); } while(0)

#define CAN_FRAME_SIZE			sizeof(struct can_frame)

/* Operation modes */
typedef enum can_mode {
	MODE_RECEIVER = 0,
	MODE_TRANSMITTER,
} can_mode_t;

/* Used for the configuration varaible st.verbose */
typedef enum verbosity_t {
	ERROR = 0,
	WARNING,
	INFO,
	DEBUG,
} verbosity_t;
static verbosity_t selected_verbosity;

/* Internal structure */
struct opts_t {
	char *iface;
	canid_t *canids;
	double *tx_rates;
	int ids;
	int extended;
	unsigned char pattern;
	int same_pattern;
	can_mode_t mode;
	int bytes;
	unsigned long loops;
	pthread_t *threads;
	int xdelay;
};

static struct opts_t *main_opts;

/*
 * Return the current time in seconds, using a double precision number.
 * This code is coming from NetPipe (used for the time meassurement)
 */
inline static double now(void)
{
	struct timeval tp;
	gettimeofday(&tp, NULL);
	return ((double)tp.tv_sec + (double)tp.tv_usec * 1e-6);
}

inline static void myprintf(verbosity_t level, const char *format, ...)
{
	va_list lst;
	char *marke;
	int weg = 0;

	if (level > selected_verbosity)
		return;

	switch (level) {
	case ERROR:
		marke = "ERROR";
		weg = 1;
		break;
	case WARNING:
		marke = "WARNING";
		break;
	case INFO:
		marke = "INFO";
		break;
	case DEBUG:
		marke = "DEBUG";
		break;
	default:
		marke = "UNKNOW";
		weg = 1;
		break;
	}
	printf("[ %s ] ", marke);
	va_start(lst, format);
	vprintf(format, lst);
	va_end(lst);

	if (weg)
		fflush(stdout);
}

static void print_version(void)
{
	pr_naked("CAN test application v%s\n", APP_VERSION);
}

static void print_usage(void)
{
	fprintf(stdout, "Usage: %s [OPTIONS]\n"
		"%s %s Copyright Digi International Inc.\n\n"
		"Data transfer using CAN-sockets\n"
		"\n"
		"  -m, --master       Run the test as master (trasmitter)\n"
		"  -D, --tx-delay=    Delay (in usec.) between each TX frame\n"
		"  -i, --ids=         IDs to use for the test (in hex)\n"
		"  -E, --extended     Enables the extended ID support\n"
		"  -d, --device=      Interface to use (e.g. can0)\n"
		"  -b, --bytes=       Number of the data bytes per CAN-frame\n"
		"  -l, --loops=       Number of test loops to execute\n"
		"  -p, --pattern=     Data pattern to use (in hex)\n"
		"  -v, --verbosity=   Verbosity level (3: loud | 0: quiet)\n"
		"  -V, --version      Show version and exit\n"
		"  -h, --help         Display usage information\n\n",
		APP_NAME, APP_NAME, APP_VERSION);
}

/*
 * Parse the input options and return the structure with the parse options.
 * By errors return NULL
 */
static struct opts_t *process_options(int argc, char *argv[])
{
	int opt_index, opt;
	static const char *short_options = "mD:d:i:Ed:b:l:p:v:Vh";
	struct opts_t *retval;
	char *str1, *token;
	char *savearg = NULL;
	int cnt;
	static const struct option long_options[] = {
		{"master", no_argument, NULL, 'm'},
		{"tx-delay", required_argument, NULL, 'D'},
		{"ids", required_argument, NULL, 'i'},
		{"extended", no_argument, NULL, 'E'},
		{"device", required_argument, NULL, 'd'},
		{"bytes", required_argument, NULL, 'b'},
		{"loops", required_argument, NULL, 'l'},
		{"pattern", required_argument, NULL, 'p'},
		{"verbosity", required_argument, NULL, 'v'},
		{"version", no_argument, NULL, 'V'},
		{"help", no_argument, NULL, 'h'},
		{0, 0, 0, 0},
	};

	/* Allocate the space for the internal data structure */
	retval = calloc(1, sizeof(struct opts_t));
	if (!retval) {
		pr_error("calloc failed, %s\n", strerror(errno));
		return NULL;
	}

	for (opt_index = 0;;) {
		opt = getopt_long(argc, argv, short_options, long_options, &opt_index);
		if (opt == EOF)
			break;

		switch (opt) {
		case 'm':
			retval->mode = MODE_TRANSMITTER;
			break;

		case 'D':
			retval->xdelay = atoi(optarg);
			break;

		case 'i':

			/* Obtain a copy of the passed argument first */
			savearg = malloc(strlen(optarg) + 1);
			if (!savearg) {
				pr_error("malloc failed, %s\n", strerror(errno));
				goto err_free_mem;
			}
			strncpy(savearg, optarg, strlen(optarg));

			for (cnt = 0, str1 = optarg; (token = strtok(str1, ",")); str1 = NULL)
				cnt++;

			retval->canids = calloc(cnt, sizeof(canid_t));
			retval->threads = calloc(cnt, sizeof(pthread_t));
			retval->tx_rates = calloc(cnt, sizeof(ulong));
			if (!retval->canids || !retval->threads || !retval->tx_rates) {
				pr_error("calloc failed, %s\n", strerror(errno));
				goto err_free_mem;
			}

			for (cnt = 0, str1 = savearg; (token = strtok(str1, ",")); str1 = NULL)
				*(retval->canids + cnt++) = strtol(token, NULL, 16);

			retval->ids = cnt;

			/* Free savearg */
			free(savearg);
			savearg = NULL;
			break;

		case 'd':
			retval->iface = optarg;
			break;

		case 'v':
			selected_verbosity = atoi(optarg);
			break;

		case 'b':
			retval->bytes = atoi(optarg);
			break;

		case 'l':
			retval->loops = atol(optarg);
			break;

		case 'p':
			retval->pattern = (unsigned char)strtol(optarg, NULL, 16);
			retval->same_pattern = 1;
			break;

		case 'E':
			retval->extended = 1;
			break;

		case 'V':
			print_version();
			goto err_free_mem;

		case '?':
			fprintf(stderr, "Unknown option -- %c\n", opt);
			/* FALLTHROUGH */
		case 'h':
		default:
			print_usage();
			goto err_free_mem;
		}
	}

	/* Sanity checks */
	if (retval->bytes > 8) {
		pr_error("Invalid data length %i\n", retval->bytes);
		goto err_free_mem;
	}

	if (!retval->loops) {
		pr_error("A valid number of test loops is required\n");
		goto err_free_mem;
	}

	if (!retval->iface) {
		pr_error("Need a CAN device for the test\n");
		goto err_free_mem;
	}

	/* Print the information about the started mode */
	if (retval->mode == MODE_TRANSMITTER)
		pr_naked("Running the test as TRANSMITTER\n");
	else
		pr_naked("Running the test as RECEIVER\n");

	pr_info("Testing with %i loops\n", retval->loops);
	return retval;

 err_free_mem:
	free(savearg);
	free(retval->threads);
	free(retval->canids);
	free(retval->tx_rates);
	free(retval);

	return NULL;
}

/*
 * Depending on the selected command line options, the below function configures
 * the CAN-frames in two different modes:
 * - With a modified incremented frame Id (RTR or extended ID)
 * - Modified data pattern (incremented with 0x11)
 */
static void update_frame(struct opts_t *opts, struct can_frame *frame, canid_t id,
			 unsigned char pattern)
{
	frame->can_id = (opts->extended) ? (id | CAN_EFF_FLAG) : (id);
	frame->can_dlc = opts->bytes;

	/* Now update the data bytes of the frame */
	memset(frame->data, pattern, opts->bytes);
}

/* Dump the content of a CAN-frame */
inline static void dump_frame(struct can_frame *frame, const char *name)
{
	int cnt;

	pr_naked("%s: ID 0x%03x | DLC 0x%02x | DATA ", name, frame->can_id, frame->can_dlc);
	for (cnt = 0; cnt < frame->can_dlc; cnt++)
		pr_naked("0x%02x ", frame->data[cnt]);
	pr_naked("\n");
}

/* Return zero if the frames are equal, otherwise one */
static int compare_frames(struct can_frame *rcv, struct can_frame *exp)
{
	if (rcv->can_id != exp->can_id)
		return 1;

	if (rcv->can_dlc != exp->can_dlc)
		return 1;

	return memcmp(rcv->data, exp->data, rcv->can_dlc);
}

static int xmit_frame(int sock, struct can_frame *frame)
{
	return write(sock, frame, CAN_FRAME_SIZE);
}

/* The argument must be the CAN-ID for this thread */
static void *xmit_thread(void *arg)
{
	int sock, fl;
	int family = PF_CAN, type = SOCK_RAW, proto = CAN_RAW;
	struct sockaddr_can addr;
	struct ifreq ifr;
	int loop, retop;
	canid_t id;
	struct can_frame txframe, rxframe;
	struct can_filter rfilter;
	unsigned char pattern;
	double jetzt, time_delta;
	ulong rate = 0;

	/* Create the socket first */
	if ((sock = socket(family, type, proto)) < 0) {
		pr_error("socket() failed, %s\n", strerror(errno));
		return NULL;
	}

	id = (canid_t) arg;
	/* Check ID length */
	if (id >= (1 << 29)) {
		pr_error("ID 0x%x exceeds max number of bits (normal=11, extended=29)\n");
		close(sock);
		return NULL;
	}
	if (id >= (1 << 11) && !main_opts->extended) {
		pr_error("ID 0x%x requires extended mode. Enable extended mode with '-E' option\n");
		close(sock);
		return NULL;
	}

	/*
	 * If started in receiver mode, then set the correct filter first.
	 * According to the documentation from (@TODO: Link) the filter matches if:
	 *      <received id> & mask = id & mask
	 */
	if (main_opts->mode != MODE_TRANSMITTER) {
		rfilter.can_id = id;
		rfilter.can_mask = (main_opts->extended) ? CAN_EFF_MASK : CAN_SFF_MASK;
		retop = setsockopt(sock, SOL_CAN_RAW, CAN_RAW_FILTER,
				   &rfilter, /* sizeof(struct can_filter) */ 8);
		if (retop) {
			pr_error("Socket setup failed, %s\n", strerror(errno));
			goto close_socket;
		}
	}

	strcpy(ifr.ifr_name, main_opts->iface);
	retop = ioctl(sock, SIOCGIFINDEX, &ifr);
	if (retop < 0) {
		pr_error("The IOCTL for `%s' failed, %s\n", main_opts->iface, strerror(errno));
		goto close_socket;
	}

	addr.can_ifindex = ifr.ifr_ifindex;
	addr.can_family = family;

	if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		pr_error("bind() failed, %s\n", strerror(errno));
		goto close_socket;
	}

	/* Configure the socket correctly */
	fl = fcntl(sock, F_GETFD);
	if (fcntl(sock, F_SETFD, fl | O_SYNC | O_NDELAY | O_NOCTTY | FD_CLOEXEC) == -1) {
		pr_error("Socket config failed, %s\n", strerror(errno));
		goto close_socket;
	}

	/* Set the initial pattern first */
	pattern = main_opts->pattern;
	jetzt = now();
	for (loop = 1; loop <= main_opts->loops; loop++) {

		/* Increment the pattern if not pattern was passed */
		if (!main_opts->same_pattern)
			pattern++;

		/*
		 * Update the next CAN-frame
		 * The transmitter will send the frame and the receiver expects it
		 */
		update_frame(main_opts, &txframe, id, pattern);

		/* In the transmitter mode only send the CAN-frame */
		if (main_opts->mode == MODE_TRANSMITTER) {
			pr_debug("Going to transmit the frame %i with the ID 0x%x\n",
				 loop, txframe.can_id);
			retop = xmit_frame(sock, &txframe);
			if (retop < 0) {
				pr_error("write() returned with errors, %s\n", strerror(errno));
				goto close_socket;
			} else if (retop != CAN_FRAME_SIZE) {
				pr_error("Couldn't send the %i bytes (%i sent)\n",
					 CAN_FRAME_SIZE, retop);
				goto close_socket;
			}

			if (main_opts->xdelay)
				usleep(main_opts->xdelay);
		} else {
			retop = read(sock, &rxframe, CAN_FRAME_SIZE);
			if (retop < 0) {
				pr_error("Read of CAN frame failed (loop %i), %s\n",
					 loop, strerror(errno));
				goto close_socket;
			}

			/*
			 * Since we need to wait for the transmitter, restart the
			 * internal timer
			 */
			if (loop == 1)
				jetzt = now();

			pr_debug("Frame %i with ID 0x%x received\n", loop, rxframe.can_id);
			if (compare_frames(&txframe, &rxframe)) {
				pr_error("Different CAN-frames at loop %i\n", loop);
				dump_frame(&txframe, "\t* EXPECTED");
				dump_frame(&rxframe, "\t* RECEIVED");
				goto close_socket;
			}
		}
	}

	/* Print some infos about the executed test */
	time_delta = now() - jetzt;
	if (main_opts->mode == MODE_TRANSMITTER) {
		rate = main_opts->loops * main_opts->bytes / (time_delta);
		pr_naked("ID 0x%03x : %lu Bps\n", id, rate);
	} else
		pr_naked("ID 0x%03x : %.4lg seconds\n", id, time_delta);

 close_socket:
	close(sock);
	return (void *)rate;
}


int main(int argc, char **argv)
{
	int retval = -1;
	int cnt;
	canid_t id;
	pthread_t *thr;

	/* Create the internal options */
	if (!(main_opts = process_options(argc, argv)))
		return EXIT_FAILURE;

	/* And start the test threads */
	for (cnt = 0; cnt < main_opts->ids; cnt++) {
		id = *(main_opts->canids + cnt);
		thr = main_opts->threads + cnt;
		pr_info("Starting the thread for the ID 0x%x\n", id);
		pthread_create(thr, NULL, xmit_thread, (void *)id);
	}

	/* Now wait for the threads */
	for (cnt = 0; cnt < main_opts->ids; cnt++) {

		/* @TODO: Pass the pointer for obtaining the transfer rate */
		pthread_join(*(main_opts->threads + cnt), NULL);
	}

	/* @TODO: Print some test results: min and max values, etc. */
	if (main_opts->mode == MODE_TRANSMITTER) {

	}

	retval = EXIT_SUCCESS;

	free(main_opts->tx_rates);
	free(main_opts->canids);
	free(main_opts->threads);
	free(main_opts);

	return retval;
}

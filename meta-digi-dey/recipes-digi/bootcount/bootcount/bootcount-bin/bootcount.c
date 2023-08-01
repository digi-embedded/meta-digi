/*
 * Copyright (c) 2023, Digi International Inc.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, you can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <getopt.h>
#include <stdio.h>

#include "bootcount_env.h"
#include "bootcount_nvmem.h"
#include "platform_utils.h"

#define VERSION		"1.0" GIT_REVISION

#define USAGE \
		"Bootcount utility.\n" \
		"Copyright(c) Digi International Inc.\n" \
		"\n" \
		"Version: %s\n" \
		"\n" \
		"Usage: bootcount [options] \n\n" \
		"  -p              --print          Print the current bootcount value (Default action)\n" \
		"  -s <value>      --set=<value>    Set current bootcount to a specific value.\n" \
		"  -r              --reset          Reset bootcount value to zero.\n" \
		"  -h              --help           Print help and exit\n" \
		"\n"

/*
 * Struct used to store the pointers to the methods to read/write
 * the bootcount value for each platform.
 */
struct platform_functions {
	int (*read_bootcount) (void);
	int (*write_bootcount) (unsigned int);
};

/*
 * Static list of the platforms with their corresponding methods to
 * access the bootcount.
 */
struct platform_functions platforms_functions[] = {
	[PLATFORM_CC6QP] = {read_bootcount_env, write_bootcount_env},
	[PLATFORM_CC6SBC] = {read_bootcount_env, write_bootcount_env},
	[PLATFORM_CC6UL] = {read_bootcount_nvmem, write_bootcount_nvmem},
	[PLATFORM_CC8MM] = {read_bootcount_nvmem, write_bootcount_nvmem},
	[PLATFORM_CC8MN] = {read_bootcount_nvmem, write_bootcount_nvmem},
	[PLATFORM_CC8X] = {read_bootcount_nvmem, write_bootcount_nvmem},
	[PLATFORM_CC93] = {read_bootcount_nvmem, write_bootcount_nvmem},
	[PLATFORM_CCMP13] = {read_bootcount_nvmem, write_bootcount_nvmem},
	[PLATFORM_CCMP15] = {read_bootcount_nvmem, write_bootcount_nvmem},
	[PLATFORM_UNKNOWN] = {NULL, NULL}
};

/* Global variables. */
platform_t platform;

/* Command line variables */
static bool read, write, reset = false;
static uint write_value;

/**
 * @brief Print usage information and exit the program with the specified exit value.
 *
 * @param exitval The exit status code for the program.
 */
static void usage_and_exit(int exitval) {
	fprintf(stdout, USAGE, VERSION);

	exit(exitval);
}

/**
 * @brief Parses command-line options.
 *
 * This function parses the command-line options passed to the bootcount utility.
 *
 * If no options are provided (`argc == 1`), the default action is assumed to be read,
 * and the 'read' flag is set to true.
 *
 * If invalid options or incorrect bootcount values are provided, the function prints
 * error messages and exits with failure status.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of strings containing the command-line arguments.
 */
static void parse_options(int argc, char *argv[]) {
	static int opt_index, opt;
	static const char *short_options = "ps:rh";
	static const struct option long_options[] = {
			{"print", no_argument, NULL, 'p'},
			{"set", required_argument, NULL, 's'},
			{"reset", no_argument, NULL, 'r'},
			{"help", no_argument, NULL, 'h'},
			{NULL, 0, NULL, 0}
	};
	char *endptr;

	if (argc == 1) {
		/* Consider default action is print. */
		read = true;
		return;
	}

	while (1) {
		opt = getopt_long(argc, argv, short_options, long_options, &opt_index);
		if (opt == -1)
			break;

		switch (opt) {
		case 'p':
			read = true;
			break;
		case 's':
			write = true;
			write_value = (int)strtoul(optarg, &endptr, 10);
			if (*endptr) {
				printf("Error: incorrect bootcount value\n");
				exit(EXIT_FAILURE);
			}
			break;
		case 'r':
			reset = true;
			break;
		case 'h':
			usage_and_exit(EXIT_SUCCESS);
			break;
		default:
			usage_and_exit(EXIT_FAILURE);
			break;
		}
	}
}

/**
 * @brief Main program function and entry point.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of strings containing the command-line arguments.
 *
 * @return 0 if the process finishes successfully, any other value otherwise..
 */
int main(int argc, char *argv[]) {
	int ret = 0;
	struct platform_functions *pfuncs;

	/* Read and parse command line */
	parse_options(argc, argv);

	/* Determine platform. */
	platform = get_platform();
	pfuncs = &platforms_functions[platform];

	/* Execute the requested action. */
	if (read) {
		ret = pfuncs->read_bootcount();
		if (ret >= 0) {
			printf("%d\n", ret);
			ret = 0;
		}
	} else if (write) {
		ret = pfuncs->write_bootcount(write_value);
	} else if (reset) {
		ret = pfuncs->write_bootcount(0);
	}

	return ret;
}

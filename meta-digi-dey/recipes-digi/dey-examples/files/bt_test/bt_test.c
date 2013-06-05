/*
 * bt_test.c
 *
 * Copyright (C) 2012 by Digi International Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * Description: Bluetooth example application.
 *
 * This application demonstrates how to use Bluetooth to transfer data
 * between two stations.
 *
 */

#include <errno.h>
#include <getopt.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>


#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#define APP_VERSION "1.0"
#define APP_NAME			"bt_test"

#define pr_error(...)			do { myprintf(ERROR, __VA_ARGS__); } while(0)
#define pr_warning(...)			do { myprintf(WARNING, __VA_ARGS__); } while(0)
#define pr_info(...)			do { myprintf(INFO, __VA_ARGS__); } while(0)
#define pr_debug(...)			do { myprintf(DEBUG, __VA_ARGS__); } while(0)
#define pr_naked(...)			do { printf(__VA_ARGS__); } while(0)

/* Operation modes */
typedef enum mode {
	MODE_RECEIVER = 0,
	MODE_TRANSMITTER,
} mode_type;

/* Used for the configuration variable st.verbose */
typedef enum verbosity_t {
	ERROR = 0,
	WARNING,
	INFO,
	DEBUG,
} verbosity_t;
static verbosity_t selected_verbosity;

/* Internal structure */
struct opts_t {
	mode_type mode;
	bdaddr_t server_address;
	char file[128];
	verbosity_t verbosity;
	int server_address_set;
	int s;
};

static struct opts_t *main_opts;

static void myprintf(verbosity_t level, const char *format, ...)
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
	pr_naked("Bluetooth example application v%s\n", APP_VERSION);
}

static void print_usage(void)
{
	fprintf(stdout, "Usage: %s [OPTIONS]\n"
		"%s %s Copyright Digi International Inc.\n\n"
		"Data transfer using Bluetooth\n"
		"\n"
		"  -m, --master       Run the test as master (transmitter)\n"
		"  -d, --destination= Address of message receiver\n"
		"  -f, --file=        File to send/receive\n"
		"  -v, --verbosity=   Verbosity level (3: loud | 0: quiet)\n"
		"  -V, --version      Show version and exit\n"
		"  -h, --help         Display usage information\n\n",
		APP_NAME, APP_NAME, APP_VERSION);
}

static struct opts_t *process_options(int argc, char *argv[])
{
	int opt_index, opt;
	static const char *short_options = "md:f:v:Vh";
	struct opts_t *retval;
	static const struct option long_options[] = {
		{"master", no_argument, NULL, 'm'},
		{"destination", required_argument, NULL, 'd'},
		{"file", required_argument, NULL, 'f'},
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
	retval->mode = MODE_RECEIVER;

	for (opt_index = 0;;) {
		opt = getopt_long(argc, argv, short_options, long_options, &opt_index);
		if (opt == EOF)
			break;

		switch (opt) {
		case 'm':
			retval->mode = MODE_TRANSMITTER;
			break;

		case 'd':
			if (str2ba(optarg, &retval->server_address) != 0) {
				pr_error("Invalid Bluetooth address: %s\n", optarg);
				goto err_free_mem;
			}
			retval->server_address_set = 1;
			break;

		case 'f':
			if (strlen(optarg) < (sizeof(retval->file))) {
				strcpy(retval->file, optarg);
			} else {
				pr_error("Pathname is too long\n");
				goto err_free_mem;
			}
			break;

		case 'v':
			selected_verbosity = atoi(optarg);
			break;

		case 'V':
			print_version();
			goto err_free_mem;

		case '?':
			pr_error("Unknown option -- %c\n", opt);
			/* FALLTHROUGH */
		case 'h':
		default:
			print_usage();
			goto err_free_mem;
		}
	}

	/* Sanity checks */
	if (retval->mode == MODE_TRANSMITTER) {
		if (!retval->server_address_set) {
			pr_error("You must specify a server address in master mode\n");
			goto err_free_mem;
		}
	}
	if (retval->file[0] == 0) {
		pr_error("You must specify a file for the transfer\n");
		goto err_free_mem;
	}

	/* Print the information about the started mode */
	if (retval->mode == MODE_TRANSMITTER)
		pr_naked("Running the test as TRANSMITTER\n");
	else
		pr_naked("Running the test as RECEIVER\n");

	return retval;

 err_free_mem:
	free(retval);

	return NULL;
}

int receiver_test(struct opts_t *main_opts)
{
	struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };
	char buf[1024] = { 0 };
	int s, client = -1, bytes_read;
	socklen_t opt = sizeof(rem_addr);
	int bytes_transfered = 0;
	FILE *out = fopen(main_opts->file, "wb");
	int result = EXIT_FAILURE;
	unsigned long permissions;

	if (out == NULL) {
		pr_error("Error creating %s: %s\n", main_opts->file, strerror(errno));
		return EXIT_FAILURE;
	}

	// allocate socket
	s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	if (s == -1) {
		pr_error("Error opening Bluetooth/RFCOMM socket.\n");
		pr_error("Did you forget to enable support for RFCOMM protocol\n");
		pr_error("in the kernel configuration menus?\n");
		fclose(out);
		return EXIT_FAILURE;
	}
	// bind socket to port 1 of the first available
	// local bluetooth adapter
	loc_addr.rc_family = AF_BLUETOOTH;
	loc_addr.rc_bdaddr = *BDADDR_ANY;
	loc_addr.rc_channel = (uint8_t) 1;
	bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr));
	if (s == -1) {
		pr_error("bind failed with errno=%d\n", errno);
		goto receiver_stop;
	}
	// put socket into listening mode
	if (listen(s, 1) == -1) {
		pr_error("listen failed with errno %d\n", errno);
		goto receiver_stop;
	}
	// accept one connection
	client = accept(s, (struct sockaddr *)&rem_addr, &opt);
	if (client == -1) {
		pr_error("accept failed with errno %d\n", errno);
		goto receiver_stop;
	}

	ba2str(&rem_addr.rc_bdaddr, buf);
	pr_naked("Accepted connection from %s\n", buf);
	memset(buf, 0, sizeof(buf));

	recv(client, &permissions, sizeof(permissions), 0);
	permissions = ntohl(permissions);
	// read data from the client
	do {
		bytes_read = recv(client, buf, sizeof(buf), 0);
		if (bytes_read > 0) {
			int bytes_written = 0;
			pr_info("Received %d bytes\n", bytes_read);
			do {
				int result = fwrite(&buf[bytes_written], 1, bytes_read - bytes_written, out);
				if (result < 0) {
					pr_error("Error writing data to file, err = %s\n", strerror(errno));
					goto receiver_stop;
				} else {
					pr_info("Wrote %d bytes to file\n", result);
					bytes_written += result;
				}
			} while (bytes_written < bytes_read);
			bytes_transfered += bytes_read;
		}
	} while (bytes_read > 0);
	pr_naked("Received %d bytes and wrote them to %s.\n", bytes_transfered, main_opts->file);

	result = EXIT_SUCCESS;

receiver_stop:
	if (client != -1)
		close(client);
	close(s);
	fclose(out);
	if (result == EXIT_SUCCESS) {
		chmod(main_opts->file, permissions);
	}

	return result;
}


int transmitter_test(struct opts_t *main_opts)
{
	struct sockaddr_rc addr = { 0 };
	int s, status;
	char buf[1024];
	int bytes_read;
	FILE *fd = NULL;
	int result = EXIT_FAILURE;
	struct stat file_info;
	unsigned long permissions;

	// allocate a socket
	s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	if (s == -1) {
		pr_error("Error opening Bluetooth/RFCOMM socket.\n");
		pr_error("Did you forget to enable support for RFCOMM protocol\n");
		pr_error("in the kernel configuration menus?\n");
		return EXIT_FAILURE;
	}
	// set the connection parameters (who to connect to)
	addr.rc_family = AF_BLUETOOTH;
	addr.rc_channel = (uint8_t) 1;
	memcpy(&addr.rc_bdaddr, &main_opts->server_address, sizeof(addr.rc_bdaddr));

	// connect to server

	status = connect(s, (struct sockaddr *)&addr, sizeof(addr));
	if (status != 0) {
		ba2str(&main_opts->server_address, buf);
		pr_error("Unable to connect to server %s\n", buf);
		goto transmitter_exit;
	}

	fd = fopen(main_opts->file, "rb");

	if (fd == NULL) {
		pr_error("Unable to open %s\n", main_opts->file);
		goto transmitter_exit;
	}
	main_opts->s = s;
	if (stat(main_opts->file, &file_info)) {
		pr_error("Unable to read permissions of %s\n", main_opts->file);
		goto transmitter_exit;
	}
	permissions = htonl(file_info.st_mode);
	send(s, &permissions, sizeof(permissions), 0);
	do {
		bytes_read = fread(buf, 1, sizeof(buf), fd);

		if (bytes_read > 0) {
			int bytes_written = 0;
			do {
				status = send(s, &buf[bytes_written], bytes_read - bytes_written, 0);
				if (status < 0) {
					if (errno == EAGAIN) {
						sleep(1);
					} else {
						pr_error("Encountered error %s sending to remote server\n", strerror(errno));
						goto transmitter_exit;
					}
				}
				pr_info("Sent %d bytes.\n", status);
				bytes_written += status;
			} while (bytes_written < bytes_read);
		} else {
			if (!feof(fd)) {
				pr_error("Encountered error %s reading file.\n", strerror(errno));
				goto transmitter_exit;
			}
		}
	} while ((bytes_read > 0) && (!feof(fd)));
	pr_naked("Finished sending %s\n", main_opts->file);
	result = EXIT_SUCCESS;

transmitter_exit:
	close(s);
	if (fd != NULL)
		fclose(fd);

	return result;
}

int main(int argc, char **argv)
{
	int result = EXIT_SUCCESS;

	/* Create the internal options */
	if (!(main_opts = process_options(argc, argv)))
		return EXIT_FAILURE;

	if (main_opts->mode == MODE_RECEIVER) {
		result = receiver_test(main_opts);
	} else {
		result = transmitter_test(main_opts);
	}
	return result;
}

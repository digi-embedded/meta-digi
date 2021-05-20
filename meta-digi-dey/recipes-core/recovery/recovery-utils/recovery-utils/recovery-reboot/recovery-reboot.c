/*
 * Copyright (c) 2017-2021, Digi International Inc.
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
 *
 * Description: Reboot into recovery mode setting recovery commands
 *
 */

#define _GNU_SOURCE	/* For GNU version of basename */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <recovery.h>

#define VERSION		"0.3" GIT_REVISION

#define REBOOT_TIMEOUT	10

#define CMD_RECOVERY	"recovery-reboot"
#define CMD_UPDATEFW	"update-firmware"
#define CMD_ENCRYPT	"encrypt-partitions"

#define RECOVERY_USAGE \
	"Reboot into recovery mode setting recovery commands.\n" \
	"Copyright(c) Digi International Inc.\n" \
	"\n" \
	"Version: %s\n" \
	"\n" \
	"Usage: %s [options] [<SWU-package-path>]\n\n" \
	"  -u              --update-firmware          Perform firmware update\n" \
	"  -e <partitions> --encrypt=<partitions>     Encrypt the list of provided partitions.\n" \
	"  -d <partitions> --unencrypt=<partitions>   Un-encrypt the list of provided partitions.\n" \
	"  -k [<key>]      --encryption-key[=<key>]   Set <key> as file system encryption key.\n" \
	"                                             Empty to generate a random key.\n" \
	"  -w              --wipe-update-partition    Wipe 'update' partition\n" \
	"  -T <N>          --reboot-timeout=<N>       Reboot after N seconds (default %d)\n" \
	"  -f              --force                    Force (un)encryption and key change operations.\n" \
	"                  --help                     Print help and exit\n" \
	"\n" \
	"<SWU-package-path>    Absolute path to the firmware update package\n" \
	"\n"

#define UPDATEFW_USAGE \
	"Update the firmware using the recovery reboot.\n" \
	"Copyright(c) Digi International Inc.\n" \
	"\n" \
	"Version: %s\n" \
	"\n" \
	"Usage: %s [options] <SWU-package-path>\n\n" \
	"  -k [<key>] --encryption-key[=<key>]   Set <key> as file system encryption key.\n" \
	"                                        Empty to generate a random key.\n" \
	"  -T <N>     --reboot-timeout=<N>       Reboot after N seconds (default %d)\n" \
	"  -f         --force                    Force (un)encryption and key change operations.\n" \
	"             --help                     Print help and exit\n" \
	"\n" \
	"<SWU-package-path>    Absolute path to the firmware update package\n" \
	"\n"

#define ENCRYPT_USAGE \
	"Encrypt/unencrypt partitions using the recovery reboot.\n" \
	"Copyright(c) Digi International Inc.\n" \
	"\n" \
	"Version: %s\n" \
	"\n" \
	"Usage: %s [-e <partitions>] [-d <partitions>] [options]\n\n" \
	"  -e <partitions> --encrypt=<partitions>     Encrypt the list of provided partitions.\n" \
	"  -d <partitions> --unencrypt=<partitions>   Un-encrypt the list of provided partitions.\n" \
	"  -k [<key>]      --encryption-key[=<key>]   Set <key> as file system encryption key.\n" \
	"                                             Empty to generate a random key.\n" \
	"  -T <N>          --reboot-timeout=<N>       Reboot after N seconds (default %d)\n" \
	"  -f              --force                    Force (un)encryption and key change operations.\n" \
	"                  --help                     Print help and exit\n" \
	"\n"

/* Check if application was called as update-firmware or encrypt-partitions */
#define IS_UPDATEFW(cmd)	(!strcmp(cmd, CMD_UPDATEFW))
#define IS_ENCRYPT(cmd)		(!strcmp(cmd, CMD_ENCRYPT))

/* Actual command name */
static char *cmd_name;

/* Command line options */
static char *swu_package;
static char *key = NULL;
static char *to_encrypt = NULL;
static char *to_unencrypt = NULL;
static int wipe_update;
static int update_fw;
static int set_key;
static int force;
static int reboot_timeout = REBOOT_TIMEOUT;

/*
 * Function:    usage_and_exit
 * Description: show usage information and exit with 'exitval' return value
 */
static void usage_and_exit(int exitval)
{
	if (IS_UPDATEFW(cmd_name))
		fprintf(stdout, UPDATEFW_USAGE, VERSION, CMD_UPDATEFW,
			REBOOT_TIMEOUT);
	else if (IS_ENCRYPT(cmd_name))
		fprintf(stdout, ENCRYPT_USAGE, VERSION, CMD_ENCRYPT,
			REBOOT_TIMEOUT);
	else
		fprintf(stdout, RECOVERY_USAGE, VERSION, CMD_RECOVERY,
			REBOOT_TIMEOUT);

	exit(exitval);
}

/*
 * Function:    parse_options
 * Description: parse command line options
 */
static void parse_options(int argc, char *argv[])
{
	static int opt_index, opt;
	static const char *short_options = "uk::wT:e:d:f";
	static const struct option long_options[] = {
		{"update-firmware", no_argument, NULL, 'u'},
		{"encryption-key", optional_argument, NULL, 'k'},
		{"wipe-update-partition", no_argument, NULL, 'w'},
		{"reboot-timeout", required_argument, NULL, 'T'},
		{"encrypt", required_argument, NULL, 'e'},
		{"unencrypt", required_argument, NULL, 'd'},
		{"force", no_argument, NULL, 'f'},
		{"help", no_argument, NULL, 'h'},
		{NULL, 0, NULL, 0}
	};
	char *endptr;

	if (argc == 1)
		usage_and_exit(EXIT_SUCCESS);

	while (1) {
		opt =
		    getopt_long(argc, argv, short_options, long_options,
				&opt_index);
		if (opt == -1)
			break;

		switch (opt) {
		case 'u':
			update_fw = 1;
			break;
		case 'w':
			wipe_update = 1;
			break;
		case 'k':
			set_key = 1;
			if (optarg)
				key = optarg;
			break;
		case 'T':
			reboot_timeout = (int)strtol(optarg, &endptr, 10);
			if (*endptr) {
				printf("Error: incorrect timeout argument\n");
				exit(EXIT_FAILURE);
			}
			break;
		case 'e':
			to_encrypt = optarg;
			break;
		case 'd':
			to_unencrypt = optarg;
			break;
		case 'f':
			force = 1;
			break;
		case 'h':
			usage_and_exit(EXIT_SUCCESS);
			break;
		default:
			usage_and_exit(EXIT_FAILURE);
			break;
		}
	}

	/* If command is 'update-firmware' or 'encrypt-partitions' reset the options */
	if (IS_UPDATEFW(cmd_name)) {
		update_fw = 1;
		wipe_update = 0;
		to_encrypt = NULL;
		to_unencrypt = NULL;
	} else if (IS_ENCRYPT(cmd_name)) {
		update_fw = 0;
		wipe_update = 0;
	}

	if (update_fw) {
		if (argc == (optind + 1)) {
			swu_package = argv[optind];
		} else {
			printf("Error: missing SWU package argument\n");
			exit(EXIT_FAILURE);
		}
	}
}

int main(int argc, char *argv[])
{
	int ret = 0;
	unsigned char need_reboot = 0;

	cmd_name = basename(argv[0]);
	if (!cmd_name) {
		printf("Error: basename command\n");
		goto out;
	}

	/* Read and parse command line */
	parse_options(argc, argv);

	if (set_key) {
		/* Configure recovery commands to set a fs encryption key */
		ret = set_encryption_key(key, force);
		if (ret < 0) {
			printf("Error: set_encryption_key\n");
			goto out;
		} else if (ret == 0) {
			/*
			 * Only reboot if strictly necessary, since the function
			 * might succeed without setting a recovery command
			 * (for example, if a user cancels the operation).
			 */
			need_reboot++;
		} else {
			ret = 0;
		}
	}

	if (swu_package) {
		/* Configure recovery commands to update the firmware */
		ret = update_firmware(swu_package);
		if (ret) {
			printf("Error: update_firmware\n");
			goto out;
		}
		need_reboot++;
	}

	if (wipe_update) {
		/* Configure recovery commands to format 'update' partition */
		ret = wipe_update_partition();
		if (ret) {
			printf("Error: wipe_update_partition\n");
			goto out;
		}
		need_reboot++;
	}

	if (to_encrypt || to_unencrypt) {
		/* Configure recovery commands to encrypt/unencrypt partitions */
		ret = encrypt_partitions(to_encrypt, to_unencrypt, force);
		if (ret < 0) {
			printf("Error: encrypt_partitions\n");
			goto out;
		} else if (ret == 0) {
			/*
			 * Only reboot if strictly necessary, since the function
			 * might succeed without setting a recovery command
			 * (for example, if you try to encrypt partitions that
			 * are already encrypted).
			 */
			need_reboot++;
		} else {
			ret = 0;
		}
	}

	if (need_reboot > 0) {
		/* Reboot to recovery */
		ret = reboot_recovery(reboot_timeout);
		if (ret) {
			printf("Error: reboot_recovery\n");
			goto out;
		}
	}

	printf("\nNo recovery commands configured.\n");
out:
	return ret;
}

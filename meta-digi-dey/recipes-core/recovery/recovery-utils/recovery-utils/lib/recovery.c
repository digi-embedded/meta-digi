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
 * Description: Recovery boot library
 *
 */

#define _GNU_SOURCE	/* For GNU version of basename */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/reboot.h>
#include <unistd.h>

#include "libuboot.h"

#define FILE_PREFIX	"file://"

#define OTP_CLOSED_BIT	2

/* Plain key size */
#define KEYSIZE_BYTES		32	/* 256 bits */

/*
 * Base64 encoded string size
 *
 * https://en.wikipedia.org/wiki/Base64#Padding
 */
#define BASE64_SIZE_BYTES(x)	(4 * ((x + (3 - 1)) / 3))

/* 20 partitions + 1 NULL array terminator */
#define MAX_PARTITIONS		(20 + 1)
#define MAX_LIST_LEN		2048

/* Shell command used to obtain the eMMC partition list */
#define EMMC_FDISK_CMD		"fdisk -l /dev/mmcblk0 | grep '^  *' | rev | cut -d ' ' -f1 | rev"
#define MAX_PART_NAME_LENGTH	64

#define PARTS_BLACKLIST		is_device_nand() ? nand_parts_blacklist : emmc_parts_blacklist
#define PARSE_PARTITION_INFO(x, y, z) \
	is_device_nand() ? parse_nand_partition_info(x, y, z) : parse_emmc_partition_info(x, y, z)

static char *nand_parts_blacklist[] = {
	"bootloader",
	"environment",
	"linux",
	"recovery",
	"safe",
	NULL
};

static char *emmc_parts_blacklist[] = {
	"linux",
	"recovery",
	"safe",
	NULL
};

static char *rootfs[] = { "rootfs", NULL };

/*
 * Function:    is_device_closed
 * Description: check if the device is in dualboot mode
 */
static bool is_dualboot_enabled(void)
{
	const char *var;
	bool ret = false;

	/* Parse dualboot */
	if (uboot_getenv("dualboot", &var)) {
		fprintf(stderr, "Error: getenv 'dualboot'\n");
		return false;
	}

	/* Consider dualboot not enabled if variable doesn't exist */
	if (!var)
		return false;

	/* Is dualboot enabled */
	if (!strcmp(var, "yes"))
		ret = true;

	return ret;
}

/*
 * Function:    append_recovery_command
 * Description: append configuration to the 'recovery_command' variable
 */
static int append_recovery_command(const char *value)
{
	const char *old_recovery_cmd;
	char *new_recovery_cmd;
	int rcvr_cmd_len;
	int ret = 0;

	/* Check if we are in dualboot mode */
	if (is_dualboot_enabled()) {
		fprintf(stderr, "Error: dualboot enabled recovery cannot be used\n");
		goto err;
	}

	ret = uboot_getenv("recovery_command", &old_recovery_cmd);
	if (ret) {
		fprintf(stderr, "Error: getenv 'recovery_command'\n");
		goto err;
	}

	/* Length of old recovery command (+1 for the space) */
	rcvr_cmd_len = (old_recovery_cmd) ? (strlen(old_recovery_cmd) + 1) : 0;

	/* Add new value's length + '\0' */
	rcvr_cmd_len += strlen(value) + 1;

	new_recovery_cmd = calloc(1, rcvr_cmd_len);
	if (!new_recovery_cmd) {
		fprintf(stderr, "Error: calloc 'new_recovery_cmd'\n");
		goto err;
	}

	/* Set new recovery command appending to previous value */
	if (old_recovery_cmd) {
		strcpy(new_recovery_cmd, old_recovery_cmd);
		strcat(new_recovery_cmd, " ");
	}
	strcat(new_recovery_cmd, value);

	ret = uboot_setenv("recovery_command", new_recovery_cmd);
	if (ret)
		fprintf(stderr, "Error: setenv 'recovery_command'\n");

	free(new_recovery_cmd);

err:
	return ret ? -1 : 0;
}

/*
 * Function:    is_device_closed
 * Description: check if the device is closed
 */
static int is_device_closed(void)
{
	const char *path_closed = "/proc/device-tree/digi,tf-closed";

	if (access(path_closed, F_OK) != -1)
		return 1;
	else
		fprintf(stderr,
			"Cannot check device status. Assuming open...\n");

	return 0;
}

/*
 * Function:    is_device_nand
 * Description: check if the device's storage is NAND/MTD
 */
static int is_device_nand(void)
{
	FILE *fp;
	char dump[256];
	char *root_start;
	char *root_end;
	const char *cmdline = "/proc/cmdline";

	fp = fopen(cmdline, "r");
	if (!fp)
		return 0;

	if (!fgets(dump, sizeof(dump), fp))
		return 0;

	root_start = strstr(dump, "root=");
	if (!root_start)
		return 0;
	root_end = strstr(root_start, " ");
	/* Truncate at root_end so that root_start contains the args for root */
	if (root_end)
		*root_end = '\0';

	return strstr(root_start, "ubi") || strstr(root_start, "mtd");
}

/*
 * Function:    print_open_device_warning
 * Description: print a warning if device isn't closed
 */
static void print_open_device_warning(void)
{
	if (!is_device_closed()) {
		printf("\n"
		       "  *****************************************************************\n"
		       "  * Warning: Use filesystem encryption only on CLOSED devices.    *\n"
		       "  *          Filesystem encryption on open devices is not secure. *\n"
		       "  *****************************************************************\n");
	}
}

/*
 * Function:    remove_entry
 * Description: remove a specific entry from a NULL-terminated array
 */
static void remove_entry(char **array, unsigned char index)
{
	free(array[index]);
	array[index] = array[index+1];

	while (array[index++])
		array[index] = array[index+1];
}

/*
 * Function:    subtract_array
 * Description: remove the entries of a NULL-terminated array from another NULL-terminated array
 */
static void subtract_array(char **subtract, char **from)
{
	unsigned char i, j;
	i = 0;

	while (from[i]) {
		j = 0;
		while(subtract[j]) {
			if (!strcmp(subtract[j], from[i])) {
				remove_entry(from, i);
				break;
			}
			j++;
		}

		if (subtract[j])
			continue;
		i++;
	}
}

/*
 * Function:    add_array
 * Description: add the entries of a NULL-terminated array to the end of another NULL-terminated array
 */
static int add_array(char **add, char **to, unsigned char limit)
{
	unsigned char i = 0, j = 0;

	/* Get to the end of 'to' */
	while (to[i])
		i++;

	/* Add all 'add' entries to the end of 'to' */
	while (add[j] && i < (limit - 1)) {
		to[i] = strdup(add[j++]);
		if (!to[i++])
			return -1;
	}

	to[i] = NULL;

	if (i == (limit - 1) && add[j])
		return -1;

	return 0;
}

/*
 * Function:    modify_array
 * Description: add and remove NULL-terminated array entries from another NULL-terminated array
 */
static int modify_array(char **target, char **to_add, char **to_remove, unsigned char limit)
{
	/* Remove the entries in 'to_add' that are already in 'target' */
	subtract_array(target, to_add);

	/* Remove the 'to_remove' entries from 'target' */
	subtract_array(to_remove, target);

	/* Add 'to_add' to 'target' while respecting the limit */
	return add_array(to_add, target, limit);
}

/*
 * Function:    entry_exists
 * Description: check if a string is an entry in a NULL-terminated array
 */
static int entry_exists(char *entry, char **array)
{
	unsigned char i = 0;

	while (array[i]) {
		if (!strcmp(entry, array[i++]))
			return 1;
	}

	return 0;
}

/*
 * Function:    is_subset
 * Description: check if a NULL-terminated array is a subset of another NULL-terminated array
 */
static int is_subset(char **subset, char **set)
{
	unsigned char i = 0;

	/* Return 1 if and only if all subset entries are in set */
	while (subset[i]) {
		if (!entry_exists(subset[i++], set))
			return 0;
	}

	return 1;
}

/*
 * Function:    list_to_array
 * Description: convert a list into a NULL-terminated string array, while checking against a blacklist and a superset
 */
static int list_to_array(char *list, char **array, char **blacklist,
			 char **superset, const char *delim, unsigned char limit)
{
	char *tmp = NULL;
	char *entry = NULL;
	int ret = -1;
	unsigned char i = 0;
	unsigned char j;

	if (!list)
		return 0;

	tmp = strdup(list);
	if (!tmp)
		return -1;

	/* Tokenize the list and iterate through it to build the array */
	entry = strtok(tmp, delim);
	while (entry && i < (limit - 1)) {
		/* If entry is in the blacklist, print a warning and discard it */
		if (blacklist && entry_exists(entry, blacklist)) {
			printf("Warning: encryption of partition '%s' is forbidden, skipping\n",
			       entry);
			entry = strtok(NULL, delim);
			continue;
		}

		/* If entry isn't a part of the superset, exit with an error */
		if (superset && !entry_exists(entry, superset)) {
			fprintf(stderr, "Error: partition '%s' doesn't exist\n", entry);
			goto err;
		}

		array[i] = strdup(entry);
		if (!array[i++])
			goto err;

		entry = strtok(NULL, delim);
	}

	/* Return an error if the number of entries surpasses the limit */
	if (entry && i == (limit - 1))
		goto err;

	array[i] = NULL;
	ret = 0;

	/* Remove duplicate entries */
	i = 0;
	while (array[i]) {
		j = i+1;
		while (array[j]) {
			if (!strcmp(array[j], array[i])) {
				remove_entry(array, j);
				continue;
			}
			j++;
		}
		i++;
	}
err:
	array[i] = NULL;
	free(tmp);
	return ret;
}

/*
 * Function:    array_to_list
 * Description: convert a non-empty NULL-terminated string array to a list
 */
static char *array_to_list(char **array, const char delim, unsigned int limit)
{
	char *list;
	char *tmp;
	unsigned char i = 0;
	unsigned int len;

	/* Obtain the total length of the list to allocate the exact string size */
	len = 0;
	while (array[i])
		len += strlen(array[i++]) + 1;

	/* Make sure that we don't surpass the character limit */
	if (len > limit)
		return NULL;

	list = calloc(1, len);
	if (!list)
		return NULL;

	/* Iterate through the array to build the list */
	i = 0;
	tmp = list;
	while (array[i]) {
		len = strlen(array[i]);
		memcpy(tmp, array[i], len);
		tmp += len;
		if (array[++i]) {
			*tmp = delim;
			tmp++;
		}
	}
	*tmp = '\0';

	return list;
}

/*
 * Function:    free_array
 * Description: free all entries in a NULL-terminated string array
 */
static inline void free_array(char **array)
{
	unsigned char i = 0;

	while (array[i])
		free(array[i++]);
}

/*
 * Function:    parse_nand_partition_info
 * Description: puts the NAND partition and encrypted partition lists into NULL-terminated arrays
 */
static int parse_nand_partition_info(char **parts, char **encrypted, unsigned char limit)
{
	const char *var;
	char *tmp;
	char *entry;
	char *start;
	char *end;
	int ret;
	unsigned char i = 0, j = 0;

	/* Parse mtdparts for all partition info */
	ret = uboot_getenv("mtdparts", &var);
	if (ret || !var) {
		fprintf(stderr, "Error: getenv 'mtdparts'\n");
		return ret;
	}

	tmp = strdup(var);
	if (!tmp)
		return -1;

	ret = -1;

	/* Discard first part of mtdparts to get the partition list */
	entry = strtok(tmp, ":");

	/*
	 * Parse the partition names from each mtdparts entry.
	 * Expected entry format is:
	 *
	 *         <size>(<name>)[enc]
	 */
	entry = strtok(NULL, ",");
	while (entry && i < (limit - 1)) {
		start = strchr(entry, '(');
		if (!start)
			goto err;
		start++;

		end = strchr(entry, ')');
		if (!end)
			goto err;

		parts[i] = calloc(1, (end - start) + 1);
		if (!parts[i])
			goto err;

		strncpy(parts[i], start, end - start);
		parts[i++][end - start] = '\0';

		/* If the encryption flag is found, add the part to the encrypted list */
		if (strstr(entry, ")enc")) {
			encrypted[j] = strdup(parts[i-1]);
			if (!encrypted[j++])
				goto err;
		}

		entry = strtok(NULL, ",");
	}

	/* Return an error if the number of entries surpasses the limit */
	if (entry && i == (limit - 1))
		goto err;

	ret = 0;
err:
	if (tmp)
		free(tmp);

	/* NULL-terminate the arrays */
	parts[i] = NULL;
	encrypted[j] = NULL;

	return ret ? -1 : 0;
}

/*
 * Function:    parse_emmc_partition_info
 * Description: puts the eMMC partition and encrypted partition lists into NULL-terminated arrays
 */
static int parse_emmc_partition_info(char **parts, char **encrypted, unsigned char limit)
{
	FILE *fp = NULL;
	const char *var;
	char *tmp;
	char *end;
	int ret;
	unsigned char i = 0;

	/*
	 * For now, obtain eMMC partition list via the
	 * "fdisk -l" command, just like we do in the recovery
	 * script.
	 */
	fp = popen(EMMC_FDISK_CMD, "r");
	if (!fp)
		return -1;

	ret = -1;
	tmp = calloc(1, MAX_PART_NAME_LENGTH);
	if (!tmp)
		goto err;

	/* Entries have a newline character at the end, make sure to remove it */
	while (i < (limit - 1) && fgets(tmp, MAX_PART_NAME_LENGTH, fp) != NULL) {
		end = strchr(tmp, '\n');
		parts[i] = calloc(1, (end - tmp) + 1);
		if (!parts[i])
			goto err;
		strncpy(parts[i], tmp, (end - tmp));
		parts[i++][end - tmp] = '\0';
	}

	/* Return an error if the number of partitions surpasses the limit */
	if (i == (limit - 1) && fgets(tmp, MAX_PART_NAME_LENGTH, fp) != NULL)
		goto err;

	/* Obtain encrypted partition list from the environment */
	ret = uboot_getenv("encrypted_parts_list", &var);
	if (ret) {
		fprintf(stderr, "Error: getenv 'encrypted_parts_list'\n");
		goto err;
	}

	/*
	 * If non-existing partitions have been added by hand to
	 * encrypted_parts_list, the process will fail at this point.
	 */
	ret = list_to_array(var, encrypted, emmc_parts_blacklist, parts, " ", limit);
err:
	if (fp)
		pclose(fp);
	if (tmp)
		free(tmp);

	/* NULL-terminate the parts array */
	parts[i] = NULL;

	return ret ? -1 : 0;
}

/*
 * Function:    secure_memzero
 * Description: secure memzero that is not optimized out by the compiler
 */
void secure_memzero(void *buf, size_t len)
{
	volatile uint8_t *p = (volatile uint8_t *)buf;

	while (len--)
		*p++ = 0;
}

/*
 * Function:    update_firmware
 * Description: configure recovery commands to update the firmware
 */
int update_firmware(const char *swu_path)
{
	char *fwupdate_cmd;
	int file_prefix_len = 0;
	int ret = -1;

	/* Check if we are in dualboot mode */
	if (is_dualboot_enabled()) {
		fprintf(stderr, "Error: dualboot enabled recovery cannot be used\n");
		goto err;
	}

	/* Verify input parameter */
	if (!swu_path) {
		fprintf(stderr, "Error: NULL 'swu_path'\n");
		goto err;
	}

	/* If file is local reset the path */
	if (!access(swu_path, F_OK)) {
		file_prefix_len = strlen(FILE_PREFIX);
		swu_path = basename(swu_path);
	}

	fwupdate_cmd =
	    calloc(1,
		   strlen("update_package=") + file_prefix_len +
		   strlen(swu_path) + 1);
	if (!fwupdate_cmd) {
		fprintf(stderr, "Error: calloc 'fwupdate_cmd'\n");
		goto err;
	}

	sprintf(fwupdate_cmd, "update_package=%s%s",
		file_prefix_len ? FILE_PREFIX : "", swu_path);

	ret = append_recovery_command(fwupdate_cmd);

	free(fwupdate_cmd);

err:
	return ret ? -1 : 0;
}

/*
 * Function:    reboot_recovery
 * Description: reboot into recovery mode
 */
int reboot_recovery(unsigned int reboot_timeout)
{
	int ret = 0;

	/* Check if we are in dualboot mode */
	if (is_dualboot_enabled()) {
		fprintf(stderr, "Error: dualboot enabled recovery cannot be used\n");
		goto err;
	}

	sync();

	printf("\nThe recovery commands have been properly configured and "
	       "the system will reboot into recovery mode in %d seconds "
	       "(^C to cancel).\n\n", reboot_timeout);
	fflush(stdout);
	sleep(reboot_timeout);

	/* Configure system to boot into recovery mode */
	ret = uboot_setenv("boot_recovery", "yes");
	if (ret) {
		fprintf(stderr, "Error: setenv 'boot_recovery'\n");
		goto err;
	}

	reboot(RB_AUTOBOOT);

err:
	return ret;
}

/*
 * Function:    wipe_update_partition
 * Description: configure recovery commands to format 'update' partition
 */
int wipe_update_partition(void)
{
	return append_recovery_command("wipe_update");
}

/*
 * Function:    set_encryption_key
 * Description: configure recovery commands to set a partition encryption key
 */
int set_encryption_key(char *key, unsigned char force)
{
	char *parts[MAX_PARTITIONS];
	char *encrypted[MAX_PARTITIONS];
	char *key_cmd = NULL;
	char confirmation;
	int generate_random_key = 0;
	int ret = -1;
	unsigned char i = 0;

	/* Check if we are in dualboot mode */
	if (is_dualboot_enabled()) {
		fprintf(stderr, "Error: dualboot enabled recovery cannot be used\n");
		return ret;
	}

	/* Initialize arrays */
	parts[0] = NULL;
	encrypted[0] = NULL;

	generate_random_key = (!key || strlen(key) == 0);

	if (!generate_random_key &&
	    BASE64_SIZE_BYTES(KEYSIZE_BYTES) != strlen(key)) {
		fprintf(stderr, "Error: invalid key size\n");
		goto err;
	}

	print_open_device_warning();

	if (!force) {
		/* Check if there are any currently encrypted partitions */
		ret = PARSE_PARTITION_INFO(parts, encrypted, MAX_PARTITIONS);
		if (ret) {
			fprintf(stderr, "Error: parse_partition_info\n");
			goto err;
		}

		/*
		 * Key changes with an encrypted rootfs are only possible if
		 * an update package is provided, in which case, we're already
		 * substituting the current rootfs image with another one.
		 * Because of this, there's no need to print the warning if the
		 * rootfs is the only encrypted partition in the system.
		 */
		subtract_array(rootfs, encrypted);

		/*
		 * If we have at least one encrypted partition, ask for
		 * confirmation before continuing.
		 */
		if (encrypted[0]) {
			printf("\n"
			       "  *****************************************************************\n"
			       "  * Warning: Changing the encryption key will erase the contents  *\n"
			       "  *          of all currently encrypted partitions.               *\n"
			       "  *****************************************************************\n"
			       "  Affected partitions:\n");
			while (encrypted[i])
				printf("      %s\n", encrypted[i++]);
			printf("\n  Continue? (y/n): ");
			confirmation = getchar();
			if (confirmation != 'y' && confirmation != 'Y') {
				printf("\nSkipping encryption key change\n");
				ret = 1;
				goto err;
			}
		}
	}

	key_cmd = calloc(1, strlen("encryption_key=") +
			 (generate_random_key ? 0 : strlen(key)) + 1);
	if (!key_cmd) {
		fprintf(stderr, "Error: calloc 'key_cmd'\n");
		goto err;
	}

	sprintf(key_cmd, "encryption_key=%s", generate_random_key ? "" : key);

	ret = append_recovery_command(key_cmd);

	free(key_cmd);

err:
	/* Secure delete the key buffers */
	if (!generate_random_key)
		secure_memzero(key, strlen(key));

	free_array(encrypted);
	free_array(parts);

	return ret < 0 ? -1 : ret;
}

/*
 * Function:    encrypt_partitions
 * Description: configure recovery commands to encrypt/un-encrypt provided partitions
 */
int encrypt_partitions(char *to_encrypt, char *to_unencrypt, unsigned char force)
{
	char *parts[MAX_PARTITIONS];
	char *encrypted[MAX_PARTITIONS];
	char *new_encrypted[MAX_PARTITIONS];
	char *enc_diff[MAX_PARTITIONS];
	char *unenc_diff[MAX_PARTITIONS];
	char *true_unenc_diff[MAX_PARTITIONS];
	char *new_list = NULL;
	char *cmd = NULL;

	char confirmation;
	unsigned char i = 0;

	int ret;

	/* Check if we are in dualboot mode */
	if (is_dualboot_enabled()) {
		fprintf(stderr, "Error: dualboot enabled recovery cannot be used\n");
		return 1;
	}

	/* If both lists are empty, we have nothing to do */
	if (!to_encrypt && !to_unencrypt)
		return 1;

	/* Initialize arrays */
	parts[0] = NULL;
	encrypted[0] = NULL;
	enc_diff[0] = NULL;
	unenc_diff[0] = NULL;
	true_unenc_diff[0] = NULL;
	new_encrypted[0] = NULL;

	/* Get current partition info */
	ret = PARSE_PARTITION_INFO(parts, encrypted, MAX_PARTITIONS);
	if (ret) {
		fprintf(stderr, "Error: parse_partition_info\n");
		goto err;
	}

	/* Transform the lists into arrays for easier processing */
	ret = list_to_array(to_encrypt, enc_diff, PARTS_BLACKLIST, parts, ",", MAX_PARTITIONS);
	if (ret) {
		fprintf(stderr, "Error: list_to_array 'to_encrypt'\n");
		goto err;
	}
	ret = list_to_array(to_unencrypt, unenc_diff, NULL, parts, ",", MAX_PARTITIONS);
	if (ret) {
		fprintf(stderr, "Error: list_to_array 'to_unencrypt'\n");
		goto err;
	}

	/* If both diffs are empty, return immediately. */
	if (!enc_diff[0] && !unenc_diff[0]) {
		ret = 1;
		goto err;
	}

	/*
	 * Special case: rootfs encryption is possible, but it can't be done
	 * manually. Like with the blacklisted partitions, remove any
	 * appearence of 'rootfs' from the diffs, but with a different message.
	 */
	if (entry_exists(rootfs[0], enc_diff)) {
		printf("Warning: rootfs encryption cannot be done manually, skipping\n");
		subtract_array(rootfs, enc_diff);
	}
	if (entry_exists(rootfs[0], unenc_diff)) {
		printf("Warning: rootfs unencryption cannot be done manually, skipping\n");
		subtract_array(rootfs, unenc_diff);
	}

	/*
	 * Create a copy of the encrypted parts array. We know the copy won't
	 * surpass the limit, but check the return code in case of a strdup()
	 * failure.
	 */
	ret = add_array(encrypted, new_encrypted, MAX_PARTITIONS);
	if (ret) {
		fprintf(stderr, "Error: add_array 'new_encrypted'\n");
		goto err;
	}

	/* Build the new array of encrypted parts using the diffs */
	ret = modify_array(new_encrypted, enc_diff, unenc_diff, MAX_PARTITIONS);
	if (ret) {
		fprintf(stderr, "Error: modify_array\n");
		goto err;
	}

	/*
	 * If the new encrypted parts array is equal to the old one, there's
	 * no need to set the recovery command, so return immediately.
	 */
	if (is_subset(encrypted, new_encrypted) && is_subset(new_encrypted, encrypted)) {
		ret = 1;
		goto err;
	}

	/*
	 * Print warning on open devices only if we're encrypting at least
	 * one partition.
	 */
	if (enc_diff[0])
		print_open_device_warning();

	/*
	 * At this point, we know that we have at least one partition to
	 * (un)encrypt, so ask for confirmation before continuing.
	 */
	if (!force) {
		/*
		 * Even though the unenc_diff list is sanitized at this
		 * point, it might contain partitions that are already
		 * unencrypted. Calculate the true diff between the current
		 * list and the new one.
		 */
		ret = add_array(encrypted, true_unenc_diff, MAX_PARTITIONS);
		if (ret) {
			fprintf(stderr, "Error: add_array 'true_unenc_diff'\n");
			goto err;
		}
		subtract_array(new_encrypted, true_unenc_diff);
		printf("\n"
		       "  *****************************************************************\n"
		       "  * Warning: Partition (un)encryption is a destructive operation. *\n"
		       "  *          The affected partitions' contents will be erased in  *\n"
		       "  *          the process.                                         *\n"
		       "  *****************************************************************\n"
		       "  Affected partitions:\n");

		while (enc_diff[i])
			printf("      %s\n", enc_diff[i++]);

		i = 0;
		while (true_unenc_diff[i])
			printf("      %s\n", true_unenc_diff[i++]);

		printf("\n  Continue? (y/n): ");
		confirmation = getchar();
		if (confirmation != 'y' && confirmation != 'Y') {
			printf("\nSkipping (un)encryption of partitions\n");
			ret = 1;
			goto err;
		}
	}

	ret = -1;

	/* Create comma-separated list from the encrypted parts array */
	if (new_encrypted[0]) {
		new_list = array_to_list(new_encrypted, ',', MAX_LIST_LEN);
		if (!new_list) {
			fprintf(stderr, "Error: array_to_list\n");
			goto err;
		}
	}

	cmd = calloc(1,
		     strlen("encrypt_partitions=") +
		     (new_list ? strlen(new_list) : 0) + 1);
	if (!cmd) {
		fprintf(stderr, "Error: calloc 'cmd'\n");
		goto err_cmd;
	}

	sprintf(cmd, "encrypt_partitions=%s", new_list ? new_list : "");

	ret = append_recovery_command(cmd);

	free(cmd);
err_cmd:
	if (new_list)
		free(new_list);
err:
	free_array(enc_diff);
	free_array(unenc_diff);
	free_array(true_unenc_diff);
	free_array(new_encrypted);
	free_array(encrypted);
	free_array(parts);

	return ret < 0 ? -1 : ret;
}

/*
 * Copyright (c) 2017, Digi International Inc.
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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/reboot.h>
#include <unistd.h>

#include <libubootenv/ubootenv.h>

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
	const char *path = "/sys/fsl_otp/HW_OCOTP_CFG5";
	FILE *fd = NULL;
	unsigned int value = 0;
	long open = 0;

	if ((fd = fopen(path, "r")) == NULL) {
		fprintf(stderr, "Cannot check device status. Assuming closed...\n");
		return 1;
	}

	open = (fscanf(fd, "%x", &value) == 1) && (value & OTP_CLOSED_BIT);

	fclose(fd);

	return open;
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

	sync();

	/* Configure system to boot into recovery mode */
	ret = uboot_setenv("boot_recovery", "yes");
	if (ret) {
		fprintf(stderr, "Error: setenv 'boot_recovery'\n");
		goto err;
	}

	printf("\nThe recovery commands have been properly configured and "
	       "the system will reboot into recovery mode in %d seconds "
	       "(^C to cancel).\n\n", reboot_timeout);
	fflush(stdout);
	sleep(reboot_timeout);
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
 * Function:    set_fs_encryption_key
 * Description: configure recovery commands to set a file system encryption key
 */
int set_fs_encryption_key(char *key)
{
	char *key_cmd = NULL;
	int generate_random_key = 0;
	int ret = -1;

	generate_random_key = (!key || strlen(key) == 0);

	if (!generate_random_key &&
	    BASE64_SIZE_BYTES(KEYSIZE_BYTES) != strlen(key)) {
		fprintf(stderr, "Error: invalid key size\n");
		goto err;
	}

	if (!is_device_closed()) {
		printf("\n"
		       "  *****************************************************************\n"
		       "  * Warning: Use filesystem encryption only on CLOSED devices.    *\n"
		       "  *          Filesystem encryption on open devices is not secure. *\n"
		       "  *****************************************************************\n");
	}

	key_cmd =
	    calloc(1,
		   strlen("encryption_key=") +
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

	return ret ? -1 : 0;
}

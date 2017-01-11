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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/reboot.h>
#include <unistd.h>

#include <libubootenv/ubootenv.h>

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
 * Function:    update_firmware
 * Description: configure recovery commands to update the firmware
 */
int update_firmware(const char *swu_path)
{
	char *fwupdate_cmd;
	int ret = -1;

	/* Verify input parameter */
	if (!swu_path) {
		fprintf(stderr, "Error: NULL 'swu_path'\n");
		goto err;
	}

	fwupdate_cmd =
	    calloc(1, strlen("update_package=") + strlen(swu_path) + 1);
	if (!fwupdate_cmd) {
		fprintf(stderr, "Error: calloc 'fwupdate_cmd'\n");
		goto err;
	}

	sprintf(fwupdate_cmd, "update_package=%s", swu_path);

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

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

#include <stdio.h>

#include "bootcount_env.h"
#include "libuboot.h"

/* Environment variables. */
#define ENV_VAR_UPGRADE_AVAILABLE	"upgrade_available"
#define ENV_VAR_BOOTCOUNT			"bootcount"

int read_bootcount_env() {
	int ret;
	char* endptr;
	const char *var;

	/* Obtain 'bootcount' value from environment. */
	ret = uboot_getenv(ENV_VAR_BOOTCOUNT, &var);
	if (!ret) {
		/* Convert read value to integer. */
		ret = (int)strtoul(var, &endptr, 10);
		if (*endptr) {
			printf("Error: incorrect bootcount value in environment\n");
			ret = -1;
		}
	} else {
		fprintf(stderr, "Error: could not read '%s' variable from U-Boot environment'\n", ENV_VAR_BOOTCOUNT);
		ret = -1;
	}

	free((char*)var);
	return ret;
}

int write_bootcount_env(uint count) {
	int ret;
	char value_str[5];

	/* Convert value to string. */
	snprintf(value_str, sizeof(value_str), "%u", count);
	/* Write value to environment. */
	ret = uboot_setenv(ENV_VAR_BOOTCOUNT, value_str);
	if (ret) {
		fprintf(stderr, "Error: could not write '%s' variable to U-Boot environment\n", ENV_VAR_BOOTCOUNT);
		ret = -1;
	} else if (count == 0) {
		/* Clear 'upgrade_available' variable. */
		ret = uboot_setenv(ENV_VAR_UPGRADE_AVAILABLE, NULL);
		if (ret) {
			fprintf(stderr, "Error: could not unset '%s' variable in U-Boot environment\n", ENV_VAR_UPGRADE_AVAILABLE);
			ret = -1;
		}
	} else {
		/* Set 'upgrade_available' variable to '1'. */
		snprintf(value_str, sizeof(value_str), "%u", 1);
		ret = uboot_setenv(ENV_VAR_UPGRADE_AVAILABLE, value_str);
		if (ret) {
			fprintf(stderr, "Error: could not set '%s' variable in U-Boot environment\n", ENV_VAR_UPGRADE_AVAILABLE);
			ret = -1;
		}
	}

	return ret;
}

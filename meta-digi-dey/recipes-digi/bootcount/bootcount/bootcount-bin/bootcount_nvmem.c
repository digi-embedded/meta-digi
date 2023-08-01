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

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "file_utils.h"
#include "bootcount_nvmem.h"
#include "platform_utils.h"

/* Platform struct for the bootcount nvmem access */
struct platform_nvmem {
	const char *nvmem_path;
	long bootcount_offset;
	int bootcount_size;
};

/* List of platform structs for the bootcount nvmem access */
struct platform_nvmem platforms_nvmem[] = {
	[PLATFORM_CC6QP] = {NULL, 0, 1},
	[PLATFORM_CC6SBC] = {NULL, 0, 1},
	[PLATFORM_CC6UL] = {"/sys/bus/i2c/devices/0-007e/nvram", 0, 1},
	[PLATFORM_CC8MM] = {"/sys/bus/i2c/devices/0-0063/nvram", 0, 1},
	[PLATFORM_CC8MN] = {"/sys/bus/i2c/devices/0-0063/nvram", 0, 1},
	[PLATFORM_CC8X] = {"/sys/bus/i2c/devices/0-0063/nvram", 0, 1},
	[PLATFORM_CC93] = {"/sys/bus/i2c/devices/2-0052/rv3028_nvram0/nvmem", 0, 1},
	[PLATFORM_CCMP13] = {"/sys/bus/i2c/devices/2-0052/rv3028_nvram0/nvmem", 0, 1},
	[PLATFORM_CCMP15] = {"/sys/bus/i2c/devices/6-0052/rv3028_nvram0/nvmem", 0, 1},
	[PLATFORM_UNKNOWN] = {NULL, 0, 0},
};

/* Variables. */
extern platform_t platform;

int read_bootcount_nvmem() {
	char value;
	int ret;
	struct platform_nvmem *platform_data = &platforms_nvmem[platform];

	ret = read_file(platform_data->nvmem_path, &value, platform_data->bootcount_offset, platform_data->bootcount_size);
	if (!ret) {
		return value;
	} else {
		return -1;
	}
}

int write_bootcount_nvmem(uint count) {
	char value = (char)(count & 0xFF);
	struct platform_nvmem *platform_data = &platforms_nvmem[platform];

	return write_file(platform_data->nvmem_path, &value, platform_data->bootcount_offset, platform_data->bootcount_size);
}

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
#include <string.h>

#include "file_utils.h"
#include "platform_utils.h"

#define DT_COMPATIBLE_NODE		"/proc/device-tree/compatible"

char* platform_names[] = {
	[PLATFORM_CC6QP] = "ccimx6qp",
	[PLATFORM_CC6SBC] = "ccimx6sbc",
	[PLATFORM_CC6UL] = "ccimx6ul",
	[PLATFORM_CC8MM] = "ccimx8mm",
	[PLATFORM_CC8MN] = "ccimx8mn",
	[PLATFORM_CC8X] = "ccimx8x",
	[PLATFORM_CC91] = "ccimx91",
	[PLATFORM_CC93] = "ccimx93",
	[PLATFORM_CCMP13] = "ccmp13",
	[PLATFORM_CCMP15] = "ccmp15",
	[PLATFORM_UNKNOWN] = "unknown"
};

/**
 * @brief Checks if the running platform matches the given platform name.
 *
 * The running platform is determined by reading the corresponding entry from
 * the device tree.
 *
 * @param platform_name The name of the platform to check.
 *
 * @return true if the given platform name matches the running one, false otherwise.
 */
platform_t get_platform() {
	FILE *fd;
	char buffer[100];
	int bytes_read = 0;
	platform_t platform = PLATFORM_UNKNOWN;

	fd = fopen(DT_COMPATIBLE_NODE, "r");
	if (fd == NULL) {
		fprintf(stderr, "No DT node " DT_COMPATIBLE_NODE "\n");
		return platform;
	}

	/* The 'compatible' node specifies multiple strings null-deliniated. The fread() will read
	 * the full file, however, strstr() will consider data up to the first null byte. The strings
	 * comparison must continue until bytes_read number is reached.
	 */
	while (feof(fd) == 0 && ferror(fd) == 0 && platform == PLATFORM_UNKNOWN) {
		if ((bytes_read = fread(buffer, 1, sizeof(buffer)-1, fd)) > 0 ) {
			buffer[bytes_read] = 0; // null-terminate the full string
			char *ptr = buffer;
			while (ptr < buffer + bytes_read) {
				platform = 0;
				while (platform < PLATFORM_UNKNOWN) {
					if (strstr(ptr, platform_names[platform]) != NULL) {
						goto end;
					}
					platform += 1;
				}
				ptr += strlen(ptr) + 1;
			}
		}
	}

end:
	fclose(fd);
	return platform;
}

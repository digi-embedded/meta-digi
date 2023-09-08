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

#ifndef PLATFORM_UTILS_H
#define PLATFORM_UTILS_H

#include <stdbool.h>

/* List of all available platforms. */
typedef enum {
	PLATFORM_CC6QP,
	PLATFORM_CC6SBC,
	PLATFORM_CC6UL,
	PLATFORM_CC8MM,
	PLATFORM_CC8MN,
	PLATFORM_CC8X,
	PLATFORM_CC93,
	PLATFORM_CCMP13,
	PLATFORM_CCMP15,
	PLATFORM_UNKNOWN
} platform_t;

/* List of all platform names. */
extern char* platform_names[];

/**
 * @brief Retrieve the running platform.
 *
 * The running platform is determined by reading the corresponding entry from
 * the device tree.
 *
 * @return The running platform, 'PLATFORM_UNKNOWN' if the platform cannot be determined.
 */
platform_t get_platform();

#endif /* PLATFORM_UTILS_H */

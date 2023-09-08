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

#ifndef BOOTCOUNT_ENV_H
#define BOOTCOUNT_ENV_H

#include <stdbool.h>
#include <stdlib.h>

/**
 * @brief Read the 'bootcount' variable from the U-Boot environment.
 *
 * This function retrieves the 'bootcount' value from the U-Boot environment and
 * converts it to an integer.
 *
 * @return The 'bootcount' value as an integer on success, -1 on error.
 */
int read_bootcount_env();

/**
 * @brief Set the 'bootcount' variable in the U-Boot environment.
 *
 * This function sets the 'bootcount' value to the given one in the U-Boot environment.
 *
* @param count The new bootcount value to set.
 *
 * @return 0 on success, -1 on error.
 */
int write_bootcount_env(uint count);

#endif /* BOOTCOUNT_ENV_H */

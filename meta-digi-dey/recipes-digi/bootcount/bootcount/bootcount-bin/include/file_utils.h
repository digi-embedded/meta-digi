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

#ifndef FILE_UTILS_H
#define FILE_UTILS_H

/**
 * @brief Reads data from a binary file into the provided buffer.
 *
 * This function opens the specified binary file in read mode and reads up to
 * 'num_bytes' of data starting at 'offset' into the provided 'buffer'.
 *
 * @param path The path to the binary file to read.
 * @param buffer The buffer to store the read data.
 * @param offset The offset from where to start reading in the file.
 * @param num_bytes The number of bytes to read.
 *
 * @return 0 on success, -1 on error.
 */
int read_file(const char* path, char* buffer, long offset, int num_bytes);

/**
 * @brief Writes data to a binary file at the specified offset.
 *
 * This function opens the specified binary file in read/write mode and writes up to
 * 'num_bytes' of data from the provided 'data' buffer starting at 'offset' in the file.
 *
 * @param path The path to the binary file to write to.
 * @param data The buffer containing the data to write to the file.
 * @param offset The offset in the file where to start writing.
 * @param num_bytes The number of bytes to write.
 *
 * @return 0 on success, -1 on error.
 */
int write_file(const char* path, const char* data, long offset, int num_bytes);

#endif /* FILE_UTILS_H */

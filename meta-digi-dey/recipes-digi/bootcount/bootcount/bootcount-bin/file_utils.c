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

#include "file_utils.h"

int read_file(const char* path, char* buffer, long offset, int num_bytes) {
	FILE* file;
	int ret = -1;

	/* Sanity check. */
	if (path == NULL) {
		fprintf(stderr, "Error opening file: path is NULL");
		return ret;
	}

	file = fopen(path, "rb");
	if (!file) {
		fprintf(stderr, "Error opening file");
		return ret;
	}

	fseek(file, offset, SEEK_SET);
	ret = fread(buffer, sizeof(char), num_bytes, file);
	if (ret != num_bytes) {
		printf("Error reading from file '%s'\n", path);
		ret = -1;
	} else {
		ret = 0;
	}

	fclose(file);
	return ret;
}

int write_file(const char* path, const char* data, long offset, int num_bytes) {
	FILE* file;
	int ret = -1;

	/* Sanity check. */
	if (path == NULL) {
		fprintf(stderr, "Error opening file: path is NULL");
		return ret;
	}

	file = fopen(path, "r+b");
	if (!file) {
		fprintf(stderr, "Error opening file");
		return ret;
	}

	fseek(file, offset, SEEK_SET);
	ret = fwrite(data, sizeof(char), num_bytes, file);
	if (ret != num_bytes) {
		printf("Error writing to file '%s'\n", path);
		ret = -1;
	} else {
		ret = 0;
	}

	fclose(file);
	return ret;
}

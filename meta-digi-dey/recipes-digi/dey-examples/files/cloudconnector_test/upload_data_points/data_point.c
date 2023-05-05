/*
 * Copyright (c) 2017-2023 Digi International Inc.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Digi International Inc., 9350 Excelsior Blvd., Suite 700, Hopkins, MN 55343
 * ===========================================================================
 */

#include <limits.h>
#include <stdio.h>
#include <time.h>

#include "data_point.h"

#define STREAM_NAME		"incremental"

/*
 * get_incremental() - Retrieves an incremental value each time
 */
static int get_incremental(void)
{
	static int incremental = -1;

	if (incremental == INT_MAX)
		incremental = 0;
	else
		incremental++;

	log_debug("Incremental = %d\n", incremental);

	return incremental;
}

/*
 * get_timestamp() - Get the current timestamp of the system
 *
 * Return: The timestamp of the system.
 */
static ccapi_timestamp_t *get_timestamp(void)
{
	ccapi_timestamp_t *timestamp = NULL;
	size_t len = strlen("2016-09-27T07:07:09.546Z") + 1;
	char *date = NULL;
	time_t now;

	timestamp = (ccapi_timestamp_t*) malloc(sizeof(ccapi_timestamp_t));
	if (timestamp == NULL)
		return NULL;

	date = (char*) malloc(sizeof(char) * len);
	if (date == NULL) {
		free(timestamp);
		return NULL;
	}

	time(&now);
	if (strftime(date, len, "%FT%TZ", gmtime(&now)) > 0) {
		timestamp->iso8601 = date;
	} else {
		free(date);
		timestamp->iso8601 = NULL;
	}

	return timestamp;
}

ccapi_dp_error_t init_data_stream(ccapi_dp_collection_handle_t *dp_collection)
{
	ccapi_dp_collection_handle_t collection;
	ccapi_dp_error_t dp_error;

	dp_error = ccapi_dp_create_collection(&collection);
	if (dp_error != CCAPI_DP_ERROR_NONE) {
		log_error("ccapi_dp_create_collection() error %d\n", dp_error);
		return dp_error;
	} else {
		*dp_collection = collection;
	}

	dp_error = ccapi_dp_add_data_stream_to_collection_extra(collection,
			STREAM_NAME, "int32 ts_iso", "counts", NULL);
	if (dp_error != CCAPI_DP_ERROR_NONE) {
		log_error("ccapi_dp_add_data_stream_to_collection_extra() error %d\n",
				dp_error);
		free(collection);
	}

	return dp_error;
}

ccapi_dp_error_t add_data_point(ccapi_dp_collection_handle_t dp_collection)
{
	ccapi_dp_error_t dp_error;

	ccapi_timestamp_t *timestamp = get_timestamp();

	dp_error = ccapi_dp_add(dp_collection, STREAM_NAME, get_incremental(), timestamp);
	if (dp_error != CCAPI_DP_ERROR_NONE) {
		log_error("ccapi_dp_add() failed with error: %d\n", dp_error);
	}

	if (timestamp != NULL) {
		if (timestamp->iso8601 != NULL) {
			free((char *) timestamp->iso8601);
			timestamp->iso8601 = NULL;
		}
		free(timestamp);
		timestamp = NULL;
	}

	return dp_error;
}

ccapi_dp_error_t send_data_stream(ccapi_dp_collection_handle_t dp_collection)
{
	ccapi_dp_error_t dp_error;

	log_debug("%s", "Sending Data Stream with new incremental value\n");

	dp_error = ccapi_dp_send_collection(CCAPI_TRANSPORT_TCP, dp_collection);
	if (dp_error != CCAPI_DP_ERROR_NONE) {
		log_error("ccapi_dp_send_collection() error %d\n", dp_error);
	}

	return dp_error;
}

ccapi_dp_error_t destroy_data_stream(ccapi_dp_collection_handle_t dp_collection)
{
	log_debug("%s", "Destroying Data Stream\n");
	return ccapi_dp_destroy_collection(dp_collection);
}

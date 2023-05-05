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

#include <signal.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "device_request.h"

#if !(defined UNUSED_ARGUMENT)
#define UNUSED_ARGUMENT(a)	(void)(a)
#endif

/*
 * get_time_cb() - Data callback for 'get_time' device requests
 *
 * @target:		Target ID of the device request (get_time).
 * @transport:		Communication transport used by the device request.
 * @req_buf_info:	Buffer containing the device request.
 * @resp_buf_info:	Buffer to store the answer of the request.
 *
 * Logs information about the received request and executes the corresponding
 * command.
 */
ccapi_receive_error_t get_time_cb(char const *const target,
				ccapi_transport_t const transport,
				ccapi_buffer_info_t const *const req_buf_info,
				ccapi_buffer_info_t *const resp_buf_info)
{
	time_t t = time(NULL);
	char *time_str = ctime(&t);

	UNUSED_ARGUMENT(req_buf_info);
	log_debug("%s: target='%s' - transport='%d'", __func__, target, transport);

	resp_buf_info->length = snprintf(NULL, 0, "Time: %s", time_str);
	resp_buf_info->buffer = calloc(resp_buf_info->length + 1, sizeof(char));
	if (resp_buf_info->buffer == NULL) {
		log_error("%s: resp_buf_info calloc error", __func__);
		return CCAPI_RECEIVE_ERROR_INSUFFICIENT_MEMORY;
	}

	resp_buf_info->length = sprintf(resp_buf_info->buffer, "Time: %s", time_str);

	return CCAPI_RECEIVE_ERROR_NONE;
}

/*
 * get_time_status_cb() - Status callback for 'get_time' device requests
 *
 * @target:		Target ID of the device request (get_time)
 * @transport:		Communication transport used by the device request.
 * @resp_buf_info:	Buffer containing the response data.
 * @receive_error:	The error status of the receive process.
 *
 * This callback is executed when the response process has finished. It doesn't
 * matter if everything worked or there was an error during the process.
 *
 * Cleans and frees the response buffer.
 */
void get_time_status_cb(char const *const target,
			ccapi_transport_t const transport,
			ccapi_buffer_info_t *const resp_buf_info,
			ccapi_receive_error_t receive_error)
{
	log_debug("%s: target='%s' - transport='%d' - error='%d'",
		  __func__, target, transport, receive_error);

	/* Free the response buffer */
	if (resp_buf_info != NULL)
		free(resp_buf_info->buffer);
}

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

#define MAX_RESPONSE_SIZE	256

#if !(defined UNUSED_ARGUMENT)
#define UNUSED_ARGUMENT(a)	(void)(a)
#endif

/*
 * get_time_cb() - Data callback for 'get_time' device requests
 *
 * @target:					Target ID of the device request (get_time).
 * @transport:				Communication transport used by the device request.
 * @request_buffer_info:	Buffer containing the device request.
 * @response_buffer_info:	Buffer to store the answer of the request.
 *
 * Logs information about the received request and executes the corresponding
 * command.
 */
void get_time_cb(char const *const target,
		ccapi_transport_t const transport,
		ccapi_buffer_info_t const *const request_buffer_info,
		ccapi_buffer_info_t *const response_buffer_info)
{
	UNUSED_ARGUMENT(request_buffer_info);
	log_debug("get_time_cb(): target='%s' - transport='%d'", target, transport);

	response_buffer_info->buffer = malloc(sizeof(char) * MAX_RESPONSE_SIZE + 1);
	if (response_buffer_info->buffer == NULL) {
		log_error("%s\n", "get_time_cb(): response_buffer_info malloc error");
		return;
	}

	time_t t = time(NULL);
	response_buffer_info->length = snprintf(response_buffer_info->buffer,
			MAX_RESPONSE_SIZE, "Time: %s", ctime(&t));
}

/*
 * get_time_status_cb() - Status callback for 'get_time' device requests
 *
 * @target:					Target ID of the device request (get_time)
 * @transport:				Communication transport used by the device request.
 * @response_buffer_info:	Buffer containing the response data.
 * @receive_error:			The error status of the receive process.
 *
 * This callback is executed when the response process has finished. It doesn't
 * matter if everything worked or there was an error during the process.
 *
 * Cleans and frees the response buffer.
 */
void get_time_status_cb(char const *const target,
			ccapi_transport_t const transport,
			ccapi_buffer_info_t *const response_buffer_info,
			ccapi_receive_error_t receive_error)
{
	log_debug(
			"get_time_status_cb(): target='%s' - transport='%d' - error='%d'",
			target, transport, receive_error);

	/* Free the response buffer */
	if (response_buffer_info != NULL)
		free(response_buffer_info->buffer);
}

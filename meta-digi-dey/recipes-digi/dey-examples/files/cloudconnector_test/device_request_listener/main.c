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

#include <cloudconnector.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "device_request.h"

static void sigint_handler(int signum)
{
	log_debug("%s: received signal %d to close Cloud connection.",
		 __func__, signum);

	exit(0);
}

static void graceful_shutdown(void)
{
	stop_cloud_connection();
}

static void add_sigkill_signal(void)
{
	struct sigaction new_action;
	struct sigaction old_action;

	atexit(graceful_shutdown);

	/* Setup signal hander. */
	new_action.sa_handler = sigint_handler;
	sigemptyset(&new_action.sa_mask);
	new_action.sa_flags = 0;
	sigaction(SIGINT, NULL, &old_action);
	if (old_action.sa_handler != SIG_IGN)
		sigaction(SIGINT, &new_action, NULL);
}


/*
 * Use the following SCI request to test this example (insert your Device ID):
 *
 * <sci_request version="1.0">
 *   <data_service>
 *     <targets>
 *       <device id="00000000-00000000-00000000-00000000"/>
 *     </targets>
 *     <requests>
 *       <device_request target_name="get_time"/>
 *     </requests>
 *   </data_service>
 * </sci_request>
 *
 */

int main(void)
{
	cc_init_error_t init_error;
	cc_start_error_t start_error;
	ccapi_receive_error_t receive_error;

	add_sigkill_signal();

	init_error = init_cloud_connection(NULL);
	if (init_error != CC_INIT_ERROR_NONE) {
		log_error("Cannot initialize cloud connection, error %d", init_error);
		return EXIT_FAILURE;
	}

	start_error = start_cloud_connection();
	if (start_error != CC_START_ERROR_NONE) {
		log_error("Cannot start cloud connection, error %d", start_error);
		return EXIT_FAILURE;
	}

	receive_error = ccapi_receive_add_target(TARGET_GET_TIME, get_time_cb,
			get_time_status_cb, 0);
	if (receive_error != CCAPI_RECEIVE_ERROR_NONE) {
		log_error("Cannot register target '%s', error %d", TARGET_GET_TIME,
				receive_error);
		return EXIT_FAILURE;
	}

	printf("Waiting for Remote Manager request...\n");
	printf("Press a key to exit\n");
	getchar();

	return EXIT_SUCCESS;
}

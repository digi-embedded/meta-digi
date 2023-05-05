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
#include <stdlib.h>
#include <unistd.h>
#include <cloudconnector/cloudconnector.h>

#include "data_point.h"

#define DP_SLEEP_TIME 5
#define DP_NUMBER     10

static int running = 1;
static ccapi_dp_collection_handle_t dp_collection;

static void sigint_handler(int signum)
{
	log_debug("sigint_handler(): received signal %d to close Cloud connection.\n", signum);

	exit(0);
}

static void graceful_shutdown(void)
{
	if (running == 1) {
		destroy_data_stream(dp_collection);
	}

	running = 0;
	stop_cloud_connection();
	wait_for_ccimp_threads();
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


int main(void)
{
	cc_init_error_t init_error;
	cc_start_error_t start_error;
	ccapi_dp_error_t dp_error;
	int i;

	add_sigkill_signal();

	init_error = init_cloud_connection(NULL);
	if (init_error != CC_INIT_ERROR_NONE) {
		log_error("Cannot initialize cloud connection, error %d\n", init_error);
		return EXIT_FAILURE;
	}

	start_error = start_cloud_connection();
	if (start_error != CC_START_ERROR_NONE) {
		log_error("Cannot start cloud connection, error %d\n", start_error);
		return EXIT_FAILURE;
	}

	dp_error = init_data_stream(&dp_collection);
	if (dp_error != CCAPI_DP_ERROR_NONE) {
		log_error("Cannot initialize data stream, error %d\n", start_error);
		return EXIT_FAILURE;
	}

	running = CCAPI_TRUE;
	while (running != CCAPI_FALSE) {

		/* Collect DP_NUMBER data points sampled each DP_SLEEP_TIME seconds */
		for (i = 0; i < DP_NUMBER; i++) {
			dp_error = add_data_point(dp_collection);

			if (dp_error != CCAPI_DP_ERROR_NONE) {
				log_error("Cannot add data point, error %d\n", start_error);
				i--;
			}

			sleep(DP_SLEEP_TIME);
		}

		/* Send the block of collected data points */
		dp_error = send_data_stream(dp_collection);
		if (dp_error != CCAPI_DP_ERROR_NONE)
			log_error("Cannot send data stream, error %d\n", start_error);
	}

	return EXIT_SUCCESS;
}

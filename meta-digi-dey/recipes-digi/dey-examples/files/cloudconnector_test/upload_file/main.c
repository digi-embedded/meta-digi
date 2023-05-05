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

#define UNUSED_ARGUMENT(a)	(void)(a)

#define STREAM_NAME		"examples/uploaded_file"
#define UPLOAD_FILE		"/etc/build"

static void sigint_handler(int signum)
{
	log_debug("%s: received signal %d to close Cloud connection.",
		  __func__, signum);

	exit(0);
}

static void graceful_shutdown(void)
{
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

int main(int argc, char *argv[])
{
	cc_init_error_t init_error;
	cc_start_error_t start_error;
	ccapi_dp_b_error_t send_error;

	UNUSED_ARGUMENT(argc);

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

	send_error = ccapi_dp_binary_send_file(CCAPI_TRANSPORT_TCP, UPLOAD_FILE, STREAM_NAME);
	if (send_error != CCAPI_DP_B_ERROR_NONE) {
		log_error("%s failed, error %d", __func__, send_error);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

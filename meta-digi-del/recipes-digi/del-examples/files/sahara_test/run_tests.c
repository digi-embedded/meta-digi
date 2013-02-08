/*
 * Copyright 2005-2009 Freescale Semiconductor, Inc. All rights reserved.
 */

/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

/*!
 * @file run_tests.c
 *
 * @brief Contains main entry point and support code for FSL SHW API tests.
 *
 * This file is common between user- and kernel-mode testing, as are all of the
 * individual test suites.
 *
 * @ifnot STANDALONE
 * @ingroup MXCSAHARA2_TEST
 * @endif
 */
#include "api_tests.h"
#include "apihelp.h"

/*! Maximum number of bytes to output when a comparison fails. */
#define MAX_DUMP 20

void run_tests(fsl_shw_uco_t * user_ctx, const char *test_string,
	       uint32_t * total_passed_count, uint32_t * total_failed_count)
{
	unsigned passed_count = 0;
	unsigned failed_count = 0;
	int type;

	while ((type = *test_string++)) {
		switch (type) {
		case 012:
		case 015:
			/* ignore CR/LF */
			break;
		case 'a':	/* Should this be 'c' for CCM */
			run_gen_encrypt(user_ctx, &passed_count, &failed_count);
			break;
		case 'c':
			run_callback(user_ctx, &passed_count, &failed_count);
			break;
		case 'C':
			show_capabilities(user_ctx, &passed_count,
					  &failed_count);
			break;
		case 'd':
			run_dryice(user_ctx, &passed_count, &failed_count);
			break;
		case 'F':	/* Full test suite */
			/* Note recursive invocation. */
			run_tests(user_ctx, "CagshcmrwWzd", &passed_count,
				  &failed_count);
			break;
		case 'g':
			run_result(user_ctx, &passed_count, &failed_count);
			break;
		case 'h':
			run_hash(user_ctx, &passed_count, &failed_count);
			break;
		case 'm':
			run_hmac1(user_ctx, &passed_count, &failed_count);
			run_hmac2(user_ctx, &passed_count, &failed_count);
			break;
#ifdef FSL_API_TEST_PKC
		case 'p':
			run_pkha(user_ctx, &passed_count, &failed_count);
			break;
#endif
		case 'r':
			run_random(user_ctx, &passed_count, &failed_count);
			break;
		case 's':
			run_symmetric(user_ctx, &passed_count, &failed_count);
			break;
		case 'w':
			run_wrap(user_ctx, &passed_count, &failed_count);
			break;
		case 'W':
			run_user_wrap(user_ctx, &passed_count, &failed_count);
			break;
		case 'z':
			run_smalloc(user_ctx, &passed_count, &failed_count);
			break;
		default:
			break;
		}
	}
	*total_passed_count += passed_count;
	*total_failed_count += failed_count;
}

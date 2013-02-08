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

/**
 * @file get_results.c
 * @brief Test code for non-blocking result retrieval in FSL SHW API
 *
 * This file contains code to test fsl_shw_get_results().
 *
 * @ifnot STANDALONE
 * @ingroup MXCSAHARA2_TEST
 * @endif
 */

#include "api_tests.h"

/** The maximum number of results to request at once */
#define RESULTS_SIZE 10


/** The number of requests to make */
#define NUM_REQUESTS 20

/** The number of bytes of random data to request.  Needs to be greater than
 *  five.
 */
#define RAND_SIZE 24

struct rand_test
{
    unsigned initiated;
    unsigned result_received;
	uint8_t cache_buf1[32];
    uint8_t random[RAND_SIZE];
	uint8_t cache_buf2[32];
};


static int check_one(uint8_t random[RAND_SIZE])
{
    if ((random[0] == 0)  && (random[1]  == 0) &&
        (random[2] == 0)  && (random[3] == 0) &&
        (random[4] == 0)  && (random[5] == 0) &&
        (random[RAND_SIZE/3] == 0)  && (random[RAND_SIZE/2] == 0) &&
        (random[RAND_SIZE/2] == 0) && (random[RAND_SIZE-1] == 0)) {
        return 1;               /* failed */
    } else {
        return 0;
    }
}


/**
 * Tests the get results operation
 *
 * @param my_ctx    User context to use
 */
void run_result(fsl_shw_uco_t* my_ctx, uint32_t* total_passed_count,
                uint32_t* total_failed_count)
{
	fsl_shw_return_t code;	/* value returned from API call */
	unsigned actual;	/* number of results actually received */
	fsl_shw_result_t results[RESULTS_SIZE];	/* place to put results */
	uint32_t loop;		/* number of iterations in verify loop */
	unsigned int launched_count = 0;
	unsigned int received_count = 0;
	struct rand_test *tests;
	unsigned passed = 1;	/* boolean */

	printf("\nTest: GET RESULTS\n");

	tests = malloc(NUM_REQUESTS * sizeof(*tests));

	/* clear random data to zeros; sets initiated and result_received FALSE */
	for (loop = 0; loop < NUM_REQUESTS; loop++) {
		memset(tests[loop].random, 0, RAND_SIZE);
		tests[loop].initiated = 0;
		tests[loop].result_received = 0;
	}

	fsl_shw_uco_clear_flags(my_ctx, FSL_UCO_BLOCKING_MODE);

	/* This loop will have to be fixed when pool size is enforced. */

	for (loop = 0; loop < NUM_REQUESTS; loop++) {

		fsl_shw_uco_set_reference(my_ctx, loop);
		code =
		    fsl_shw_get_random(my_ctx, RAND_SIZE, tests[loop].random);
		if (code != FSL_RETURN_OK_S) {
			printf("fsl_shw_random(), on call %d returned %s\n",
			       loop, fsl_error_string(code));
			passed = 0;
		} else {
			launched_count++;
			tests[loop].initiated = 1;
		}
	}

    loop = 0;
	/* Now go retrieve the results */
	while ((received_count < launched_count)
	       && (loop++ < (1000 * NUM_REQUESTS))) {
		unsigned i;

		usleep(5);	/* microseconds */

        code =
		    fsl_shw_get_results(my_ctx, RESULTS_SIZE, results, &actual);
		if (code != FSL_RETURN_OK_S) {
			passed = 0;
			printf("fsl_shw_get_results() returned %s\n",
			       fsl_error_string(code));
			break;	/* get out of while() */
		}else{
			if (actual > 0)
			{
				printf("%d results received\n", actual);


			 /* and loop over each result received. */
			 for (i = 0; i < actual; i++) {
				unsigned testno =
				    fsl_shw_ro_get_reference(results + i);
				received_count++;
				if ((testno >= NUM_REQUESTS) ||
				    (!tests[testno].initiated) ||
				    tests[testno].result_received) {
					passed = 0;
					printf
					    ("result for bad reference %d received\n",
					     testno);
				} else {
					tests[testno].result_received = 1;
					if ((code =
					     fsl_shw_ro_get_status(results +
								   i)) !=
					    FSL_RETURN_OK_S) {
						printf
						    ("result %d(%d) returned error %s\n",
						     testno, i,
						     fsl_error_string(code));
						passed = 0;
					}
					if (check_one(tests[testno].random)) {
						printf
						    ("result %d values are not good\n",
						     testno);
						passed = 0;
					}
			    }
			 }

			} /* for each result received */

		}
	}

	for (loop = 0; loop < NUM_REQUESTS; loop++) {
		if (!tests[loop].result_received) {
			printf("result never received for test %d\n", loop);
			passed = 0;
		}
	}

	if (passed) {
		printf("GET RESULTS: Passed\n\n");
		*total_passed_count += 1;
	} else {
		printf("GET RESULTS: Failed\n\n");
		*total_failed_count += 1;
	}

	if (tests != NULL) {
		free(tests);
	}

    fsl_shw_uco_set_flags(my_ctx, FSL_UCO_BLOCKING_MODE);
}

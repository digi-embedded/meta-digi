/*
 * Copyright 2005-2006 Freescale Semiconductor, Inc. All rights reserved.
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
 * @file callback.c
 * @brief Test code for non-blocking Callback feature.
 *
 * @ifnot STANDALONE
 * @ingroup MXCSAHARA2_TEST
 * @endif
 */

#include "api_tests.h"

/*
 * Note to Maintainers:
 *
 * The use of global variables is inappropriate, as there is only one copy of
 * them when this code is compiled into the kernel.  Therefore, the only
 * communication between mainline code and the callback function is through the
 * structure pointed to by the reference.
 */

/** The maximum number of results to request at once */
#define RESULTS_SIZE 10

/** The number of bytes of random data to request.  Needs to be greater than
 *  five. */
#define RAND_SIZE 24


/**
 * Values to control and monitor an instance of a fsl_shw_get_random() call.
 * Purpose is to know whether the asynchronous results were returned properly.
 */
struct rand_test
{
    struct rand_test* self;     /**< Pointer to this struct, as check.  */
    unsigned initiated;         /**< Whether fsl_shw_get_random() call
                                   was kicked off */
    unsigned result_received;   /**< Whether fsl_shw_get_results() showed that
                                   this call finished.*/
    fsl_shw_return_t result;    /**< The result code fsl_shw_get_results() */
    int testno;                 /**< The test number (used as verification */
    unsigned random_ok;         /**< Whether data and @c random is OK */
    unsigned unexpected_result; /**< Whether @c result is OK */
    uint8_t cache_buf1[32];     /**< isolate random to cache line */
    uint8_t random[RAND_SIZE];  /**< The buffer for the random data */
    uint8_t cache_buf2[32];     /**< end isolate random to cache line */
};


static int run_test1(fsl_shw_uco_t* my_ctx, volatile struct rand_test tests[]);
static int run_test2(fsl_shw_uco_t* my_ctx, volatile struct rand_test tests[]);
static void test_callback_fn(fsl_shw_uco_t* uco);


/**
 * Check one random number retrieval to see whether random number looks like
 * it was actually retrieved.
 *
 * @param random   Location of the bytes of random data to verify.  Length
 *                 is RAND_SIZE.
 *
 * @return 0 if OK, non-zero if number is bad.
 */
static int check_one(uint8_t random[RAND_SIZE])
{
    if ((random[0] == 0)  && (random[1]  == 0) &&
        (random[2] == 0)  && (random[3] == 0) &&
        (random[4] == 0)  && (random[5] == 0) &&
        (random[RAND_SIZE/3] == 0)  && (random[RAND_SIZE/2] == 0) &&
        (random[RAND_SIZE-1] == 0)) {
        return 1;               /* failed */
    } else {
        return 0;
    }
}


/**
 * Internal function to initialize array of test structures.
 *
 * All values are set to 0 except for 'testno', which is set to the
 * structure's array index.
 *
 * @param tests        Pointer to beginning of array
 * @param num_tests    Number of elements in array.
 *
 * @return void
 */
static void clear_test_data(struct rand_test* tests, uint32_t num_tests)
{
    uint32_t loop;

    for (loop = 0; loop < num_tests; loop++) {
        tests[loop].self = &tests[loop];
        tests[loop].initiated = 0;
        tests[loop].result_received = 0;
        tests[loop].testno = loop;
        tests[loop].random_ok = 0;
        tests[loop].unexpected_result = 0;
        memset((void*)tests[loop].random, 0, sizeof(tests[loop].random));
    }

}


/**
 * Tests the callback and get results operations using get random function
 *
 * @param my_ctx    User context to use
 * @param[pout] total_passed_count  Number of tests which passed.
 * @param[pout] total_failed_count  Number of tests which failed.
 *
 * @return void
 */
void run_callback(
        fsl_shw_uco_t* my_ctx,
        uint32_t* total_passed_count,
        uint32_t* total_failed_count)
{
    volatile struct rand_test* tests = malloc(RESULTS_SIZE
                                              * sizeof(struct rand_test));

    unsigned         passed;     /* boolean */

    printf("\nCallback Tests\n");

    fsl_shw_uco_clear_flags(my_ctx, FSL_UCO_BLOCKING_MODE);
    fsl_shw_uco_clear_flags(my_ctx, FSL_UCO_CALLBACK_MODE);

    clear_test_data((void*)tests, RESULTS_SIZE);

    printf("   Callback Test 1: Get block of results\n");
    passed = run_test1(my_ctx, tests);
    if (passed) {
        printf("Callback Test 1: Passed\n");
        *total_passed_count += 1;
    } else {
        printf("Callback Test 1: Failed\n");
        *total_failed_count += 1;
    }

    /* Clean up for second test */
    clear_test_data((void*)tests, RESULTS_SIZE);

    if (0) {
    printf("   Callback Test 2: Set callback for each request\n");
    passed = run_test2(my_ctx, tests);
    if (passed) {
        printf("Callback Test 2: Passed\n");
        *total_passed_count += 1;
    } else {
        printf("Callback Test 2: Failed\n");
        *total_failed_count += 1;
    }
    }

    /* set uco back back to its default */
    fsl_shw_uco_set_flags(my_ctx, FSL_UCO_BLOCKING_MODE);
    fsl_shw_uco_clear_flags(my_ctx, FSL_UCO_CALLBACK_MODE);
    fsl_shw_uco_set_callback(my_ctx, NULL);


    if (tests != NULL) {
        free((void*)tests);
    }

    printf("\n");
}


/********************************************************************
 * send multiple requests in non-blocking mode. request a callback
 * only on the last service request. The callback should be able to
 * obtain all of the results in a single "get results" call
 *******************************************************************/
static int run_test1(fsl_shw_uco_t* my_ctx, volatile struct rand_test tests[])
{
    uint32_t  loop;
    uint32_t  loop_count = 0;
    fsl_shw_return_t code;
    uint32_t kicked_off_count = 0;
    int passed = 1;             /* boolean */

    /* This loop will have to be fixed when pool size is enforced. */
    for (loop = 0; loop < RESULTS_SIZE; loop++) {
        /* on last time through loop, request callback be performed */
        if (loop == (RESULTS_SIZE - 1)) {
            fsl_shw_uco_set_callback(my_ctx, test_callback_fn);
            fsl_shw_uco_set_flags(my_ctx, FSL_UCO_CALLBACK_MODE);
        }

        /* use loop value as user reference */
        fsl_shw_uco_set_reference(my_ctx, (uint32_t)(tests+loop));
        /* mark as being intialized */
        tests[loop].initiated = 1;

        /* perform a descriptor chain generating function */
        code = fsl_shw_get_random(
                   my_ctx, sizeof(tests[loop].random),
                   (uint8_t*)tests[loop].random);

        if (code == FSL_RETURN_OK_S) {
            kicked_off_count++;
        } else {
            /* mark test as failed */
            passed = 0;
            /* un-initialize the entry here (must be initialized before the
             * interrupt goes off, so it is initilzed before the descriptor
             * chain generating function above */
            tests[loop].initiated = 0;
            printf("   fsl_shw_random(), call %d returned %s\n",
                                       loop, fsl_error_string(code));
        }
    }

    /* Stay in test until we get pass/fail feedback from callback */
    while (++loop_count < 400000) {
        uint32_t callbacks_received = 0;

        /* check that every request was serviced succesfully */
        for (loop = 0; loop < RESULTS_SIZE; loop++) {
            if (tests[loop].result_received) {
                callbacks_received++;
            }
        }
        if (callbacks_received >= kicked_off_count) {
            break;      /* got everything */
        }
        usleep(5);
    }

    /* check that every request was serviced succesfully */
    for (loop = 0; loop < RESULTS_SIZE; loop++) {
        if (tests[loop].initiated) {
            if (!tests[loop].result_received) {
                printf("result never received for test %d\n", loop);
                passed = 0;
            } else if (tests[loop].result != FSL_RETURN_OK_S) {
                printf("Bad result for result %d: %s\n", loop,
                       fsl_error_string(tests[loop].result));
            } else if (!tests[loop].random_ok) {
                printf("bad random result for test %d\n", loop);
                passed = 0;
            }
        }
    }

    fsl_shw_uco_clear_flags(my_ctx, FSL_UCO_CALLBACK_MODE);
    return passed;
}


/***************************************************************
 * send a callback request along with each service request
 **************************************************************/
static int run_test2(fsl_shw_uco_t* my_ctx, volatile struct rand_test tests[])
{
    uint32_t loop;
    uint32_t loop_count = 0;
    uint32_t kicked_off_count = 0;
    fsl_shw_return_t code;
    int all_received = 0;
    int passed = 1;             /* boolean */

    fsl_shw_uco_set_flags(my_ctx, FSL_UCO_CALLBACK_MODE);
    fsl_shw_uco_set_callback(my_ctx, test_callback_fn);

    /* This loop will have to be fixed when pool size is enforced. */
    for (loop = 0; loop < RESULTS_SIZE; loop++) {
        fsl_shw_uco_set_reference(my_ctx,  (uint32_t)(tests+loop));
        tests[loop].initiated = 1;

        code = fsl_shw_get_random(
                                  my_ctx, sizeof(tests[loop].random),
                                  (uint8_t*)tests[loop].random);

        if (code == FSL_RETURN_OK_S) {
            kicked_off_count++;
        } else {
            passed = 0;
            tests[loop].initiated = 0;
            printf("   fsl_shw_random(), call %d returned %s\n",
                   loop, fsl_error_string(code));
        }
    }

    /* Stay in test until we get pass/fail feedback from callback */
    while (++loop_count < 8000000) {
        uint32_t callbacks_received = 0;

        /* check that every request was serviced succesfully */
        for (loop = 0; loop < RESULTS_SIZE; loop++) {
            if (tests[loop].result_received) {
                callbacks_received++;
            }
        }
        if (callbacks_received >= kicked_off_count) {
            all_received = 1;
            break;      /* got everything */
        }
        usleep(50);
    }

    if (!all_received) {
        printf("Not enough callbacks received\n");
        passed = 0;
    }

    /* check that every request was serviced succesfully */
    for (loop = 0; loop < RESULTS_SIZE; loop++) {
        if (tests[loop].initiated) {
            if (!tests[loop].result_received) {
                printf("result never received for test %d\n", loop);
                passed = 0;
            } else if (!tests[loop].random_ok) {
                printf("bad random result for test %d\n", loop);
                passed = 0;
            }
        }
    }

    fsl_shw_uco_clear_flags(my_ctx, FSL_UCO_CALLBACK_MODE);

    return passed;
}


static void test_callback_fn(fsl_shw_uco_t* uco)
{
    fsl_shw_result_t results[RESULTS_SIZE]; /* place to put results */
    fsl_shw_return_t code; /* value returned from API call */
    unsigned int     loop; /* number of iterations in verify loop */
    unsigned         actual;     /* number of results actually received */


    /* Request results */
    code = fsl_shw_get_results(uco, RESULTS_SIZE, results, &actual);

    /* check that results request worked */
    if (code != FSL_RETURN_OK_S) {
        printf("   fsl_shw_get_results() returned %s\n",
                fsl_error_string(code));
    } else {
        /* loop over each result received */
        for (loop = 0; loop < actual; ++loop) {
            /* check user reference value is good */
            struct rand_test* test;

            test = (void*)fsl_shw_ro_get_reference(results + loop);

            if (test == NULL) {
                printf("Test may not fail... but reference is NULL\n");
            } else if (test->self != test) {
                printf("Test may not fail... but reference is invalid\n");
            } else {
                if (!test->initiated) {

                    printf("   test case %d never initiated\n", test->testno);
                    test->unexpected_result = 1;
                } else if (test->result_received) {
                    printf("   results already received for test case %d\n",
                           test->testno);
                    test->unexpected_result = 1;
                } else {
                    /* mark that a test result was received for this ref */
                    test->result = fsl_shw_ro_get_status(results + loop);

                    /* check that a random number was likely recevied */
                    if ((test->result == FSL_RETURN_OK_S)
                        && (check_one(test->random) == 0)) {
                        test->random_ok = 1;
                    }
                    test->result_received = 1;
                }
            } /* for each result received */
        }
    }
} /* test_callback1 */

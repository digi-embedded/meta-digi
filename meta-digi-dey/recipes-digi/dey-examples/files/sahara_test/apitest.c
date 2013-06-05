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
 * @ifnot STANDALONE
 * @defgroup MXCSAHARA2_TEST Sahara2 Test
 *
 * @ingroup MXCSAHARA2
 * @endif
 */
/**
 * @file apitest.c
 * @brief User-mode test program for FSL SHW API
 *
 * This program runs suites of user-mode FSL SHW tests.  A single user context
 * for the FSL SHW API is created.  Then it, along with the suite list, is
 * passed to the test executor.
 *
 *
 * @ifnot STANDALONE
 * @ingroup MXCSAHARA2_TEST
 * @endif
 */

#include "fsl_shw.h"
#include "api_tests.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

fsl_shw_uco_t    my_ctx;


/** By default, test each suite once */
static char* test_string = "F";

/*!
 * User program wrapper for run_tests(), to test FSL SHW API.
 *
 * @param argc   The largest index of @c argv.
 * @param argv   Arguments of command invocation
 *
 * @returns   0 for success ...
 */
int main(int argc, char*argv[])
{
    fsl_shw_return_t code;
    uint32_t passed_count = 0;
    uint32_t failed_count = 0;
    int c;

    while ((c = getopt(argc, argv, "T:")) != EOF) {
        switch (c) {
        case 'T':
            test_string = optarg;
            break;
        default:
            printf("Unknown command argument: %c\n", c);
            return 1;
        }
    }

    /* Set Results Pool size to 10 */
    fsl_shw_uco_init(&my_ctx, 10);

    /* Tell hw API that we are here */
    code = fsl_shw_register_user(&my_ctx);

    if (code != FSL_RETURN_OK_S) {
        printf("fsl_shw_register_user() failed with error: %s\n",
               fsl_error_string(code));
    } else {
        /* Set my private value in ctx */
        fsl_shw_uco_set_reference(&my_ctx, 42);

        fsl_shw_uco_set_flags(&my_ctx, FSL_UCO_BLOCKING_MODE);

        run_tests(&my_ctx, test_string, &passed_count, &failed_count);
        printf("Total apitest run: %d passed, %d failed\n",
                  passed_count, failed_count);

        code = fsl_shw_deregister_user(&my_ctx);
        if (code != FSL_RETURN_OK_S) {
            printf("fsl_shw_deregister_user() failed with error: %s\n",
                   fsl_error_string(code));
        }
    }

    exit(0);

}

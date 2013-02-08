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
 * @file rand.c
 * @brief Test code for Random Number support in FSL SHW API
 *
 * This file contains vectors and code to test fsl_shw_get_random() and
 * fsl_shw_add_entropy().
 *
 * @ifnot STANDALONE
 * @ingroup MXCSAHARA2_TEST
 * @endif
 */

#include "api_tests.h"


/** Value to initialize every element of random-data output buffer. */
static const uint8_t INIT_VAL = 0x42;


/** Number of bytes of random data to request. */
static const uint32_t RANDOM_LENGTH = 256;


/**
 * "Entropy" to add to the hardware random number generation.
 */
static uint8_t entropy_values[] =
{
    0x42, 0x41, 0x40, 0x3f, 0x23, 0x00, 0x8f, 0xa1,
    0xfd, 0x01, 0x03, 0x1a, 0x40, 0xf2, 0xc1, 0xbc
};


/**
 * Length, in bytes, of entropy_values.
 */
static const uint32_t ENTROPY_LENGTH = sizeof(entropy_values);


/**
 * Test fsl_shw_get_random() by requesting RANDOM_LENGTH bytes of random data
 * from the hardware random number generator.
 *
 * @param my_ctx   User context for the API
 *
 * @return  0 = failed, non-zero = passed
 */
static int get_test(fsl_shw_uco_t* my_ctx)
{
    uint8_t* random = malloc(RANDOM_LENGTH);
    fsl_shw_return_t code;
    int passed = 0;             /* assume failure */

    memset(random, INIT_VAL, RANDOM_LENGTH);

    printf("Test Get Random\n");
    if (random == NULL) {
        printf("Skipping... memory allocation failure.\n");
    } else {
        /* Get some octets of random data */
        code = fsl_shw_get_random(my_ctx, RANDOM_LENGTH, random);
        if (code != FSL_RETURN_OK_S) {
            printf("Random: failed; fsl_shw_get_random() returned %s\n",
                   fsl_error_string(code));
        } else {
            unsigned matched = 0; /* number of elements with INIT_VAL */
            unsigned i;

            /* Compare each value returned */
            for (i = 0 ; i < RANDOM_LENGTH; i++) {
                if (random[i] == INIT_VAL) {
                    matched++;  /* same as it was before call!!! */
                }
            }
            /* No more than 10% should be the initial value... */
            if (matched > (RANDOM_LENGTH/10)) {
                unsigned i;

                printf("Random: failed: Random number does not seem random"
                       " enough.  %d values matched initial value\n", matched);
                printf("Data:");
                for (i = 0; i < RANDOM_LENGTH; i++) {
                    /* Print the occasional newline */
                    if ((i > 0) && ((i%16) == 0)) {
                        printf("\n     ");
                    }
                    printf(" %02x", random[i]);
                }
                printf("\n");
            } else {
                printf("Get Random: passed\n");
                passed = 1;
            }
        } /* else fsl_shw_get_random() succeeded */
    }

    if (random != NULL) {
        free(random);
    }

    return passed;
}


/**
 * Test fsl_shw_add_entropy() by adding data at entropy_values to the hardware
 * random number generator.
 *
 * Note that there is no way to verify whether this function did anything other
 * than return the expected result code.
 *
 * @param my_ctx   User context for the API
 *
 * @return  0 = failed, non-zero = passed
 */
static int add_test(fsl_shw_uco_t* my_ctx)
{
    fsl_shw_return_t code;
    int passed = 0;             /* assume failure */
    uint8_t* entropy = malloc(ENTROPY_LENGTH);

    printf("Test Add Entropy\n");
    if (entropy == NULL) {
        printf("Skipping... memory allocation failure.\n");
    } else {
        memcpy(entropy, entropy_values, ENTROPY_LENGTH);

        code = fsl_shw_add_entropy(my_ctx, ENTROPY_LENGTH, entropy);
        if (code != FSL_RETURN_OK_S) {
            printf("Add Entropy: failed; fsl_shw_add_entropy() returned %s\n",
                   fsl_error_string(code));
        } else {
            /* There is no way to determine, from the outside, whether this
               function actually worked.  Assume an OK means that it did. */
            printf("Add Entropy: passed\n\n");
            passed = 1;
        }
    }

    if (entropy != NULL) {
        free(entropy);
    }

    return passed;
}


/**
 * Test code to retrieve random values and add entropy
 *
 * @param my_ctx    User context to use
*/
void run_random(fsl_shw_uco_t* my_ctx, uint32_t* total_passed_count,
                uint32_t* total_failed_count)
{
    if (get_test(my_ctx)) {
        *total_passed_count += 1;
    } else {
        *total_failed_count += 1;
    }

    if (add_test(my_ctx)) {
        *total_passed_count += 1;
    } else {
        *total_failed_count += 1;
    }

    return;
}

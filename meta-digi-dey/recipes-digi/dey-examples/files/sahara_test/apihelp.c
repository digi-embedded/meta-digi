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
 * @file apihelp.c
 * @brief Support functions for the test code of FSL SHW API
 */

#include "apihelp.h"

/**
 * Compare two strings.  Print debug information if they do not compare.
 *
 * @param   model   The correct version of @a result.
 * @param   result  An output string from a test.
 * @param   length  The number of bytes of @a model, @a result to compare.
 * @param   name    A NUL-terminated string which is the name of the string
 *                  being compared.
 *
 * @return zero if the strings compare, non-zero otherwise.
 */
int compare_result(const uint8_t* model, const uint8_t* result,
                   uint32_t length, const char* name)
{
    int failed = 1;
    uint32_t i;

    for (i = 0; i < length; i++) {
        if (model[i] != result[i]) {
            break;
        }
    }
    if (i != length) {
        /* Try to dump relevant portions of data */
        uint32_t start_offset = (i > 10) ? (i - 4) : 0;
        uint32_t dump_count = (length > MAX_DUMP) ? MAX_DUMP : length;

        if ((start_offset + dump_count) > length) {
            dump_count = length - start_offset;
        }

        printf("Comparison of %s differs at offset %u of %u\n", name, i,
               length);

        printf("Good: %p (%d) ", model, start_offset);
        for (i = start_offset; i < (dump_count + start_offset); i++) {
            printf("%02x ", model[i]);
        }
        printf("\nBad:  %p (%d) ", result, start_offset);
        for (i = start_offset; i < (dump_count + start_offset); i++) {
            printf("%02x ", result[i]);
        }
        printf("\n");
    } else {
        failed = 0;
    }

    return failed;
}
#if defined(__KERNEL__)
EXPORT_SYMBOL(compare_result);
#endif


/**
 * Return a (non-allocated) string containing an interpretation of
 * an FSL SHW API error code.
 *
 * @param  code    The error code to interpret.
 *
 * @return  The associated English interpretation of the code.
 */
char* fsl_error_string(fsl_shw_return_t code)
{
    char* str;

    switch (code) {
    case FSL_RETURN_OK_S:
        str = "No error";
        break;
    case FSL_RETURN_ERROR_S:
        str = "Error";
        break;
    case FSL_RETURN_NO_RESOURCE_S:
        str = "No resource";
        break;
    case FSL_RETURN_BAD_ALGORITHM_S:
        str = "Bad algorithm";
        break;
    case FSL_RETURN_BAD_MODE_S:
        str = "Bad mode";
        break;
    case FSL_RETURN_BAD_FLAG_S:
        str = "Bad flag";
        break;
    case FSL_RETURN_BAD_KEY_LENGTH_S:
        str = "Bad key length";
        break;
    case FSL_RETURN_BAD_KEY_PARITY_S:
        str = "Bad key parity";
        break;
    case FSL_RETURN_BAD_DATA_LENGTH_S:
        str = "Bad data length";
        break;
    case FSL_RETURN_AUTH_FAILED_S:
        str = "Authentication failed";
        break;
    case FSL_RETURN_MEMORY_ERROR_S:
        str = "Memory error";
        break;
    case FSL_RETURN_INTERNAL_ERROR_S:
        str = "Internal error";
        break;
    default:
        str = "unknown value";
        break;
    }

    return str;
}
#if defined(__KERNEL__)
EXPORT_SYMBOL(fsl_error_string);
#endif


unsigned get_hash_size(fsl_shw_hash_alg_t alg)
{
    unsigned size;

     switch (alg) {
     case FSL_HASH_ALG_MD5:
         size = 16;
         break;
     case FSL_HASH_ALG_SHA1:
         size = 20;
         break;
     case FSL_HASH_ALG_SHA224:
         size = 28;
         break;
     case FSL_HASH_ALG_SHA256:
         size = 32;
         break;
     default:
         /* error ! */
         size = 1;
         break;
     }

     return size;
}
#if defined(__KERNEL__)
EXPORT_SYMBOL(get_hash_size);
#endif

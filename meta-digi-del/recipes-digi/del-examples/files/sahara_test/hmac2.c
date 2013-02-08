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
 * @file hmac2.c
 * @brief Test code for Hashed Message Authentication Codes in FSL SHW API
 *
 * This file contains vectors and code to test fsl_shw_hmac_precompute(),
 * fsl_shw_hmac(), and the functions associated with the HMAC Context Object.
 *
 * @ifnot STANDALONE
 * @ingroup MXCSAHARA2_TEST
 * @endif
 */

#include "api_tests.h"


#define MAX_CONTEXT_SIZE_BYTES 36
#define MAX_MSG_LEN 530

static uint32_t my_key[] = {0x0b0b0b0b, 0x0b0b0b0b, 0x0b0b0b0b, 0x0b0b0b0b};

static uint8_t my_msg[3][MAX_MSG_LEN] =
{
    /* from mdha_md5_hmac_chunk_data.vl with endian reversal and converting
     * hex to character (e.g., 0x34 to '4') */
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567",

    /* Lewis Carrol, Jabberwocky, Through the Looking-Glass and What
     * Alice Found There, 1872 */
    "'Twas brillig, and the slithy toves\nDid gyre and gimble in the w"
    "abe:\nAll mimsy were the borogoves,\nAnd the mome raths outgrabe.\n"
    "\n\"Beware the Jabberwock, my son!\nThe jaws that bite, the claws t"
    "hat catch!\nBeware the Jubjub bird, and shun\nThe frumious Banders"
    "natch!\"",

    /* Star Wars, New Hope, Episode IV, 1977 */
    "Episode IV\nIt is a period of civil war.\nRebel spaceships, striki"
    "ng\nfrom a hidden base, have won\ntheir first victory against\nthe "
    "evil Galactic Empire.\nDuring the battle, Rebel\nspies managed to "
    "steal secret\nplans to the Empire's\nultimate weapon, the DEATH\nST"
    "AR, an armored space\nstation with enough power to\ndestroy an ent"
    "ire planet.\nPursued by the Empire's\nsinister agents, Princess\nLe"
    "ia races home aboard her\nstarship, custodian of the\nstolen plans"
    " that can save her\npeople and restore\nfreedom to the galaxy...."
};

/* this result came from the vl test. It is the "golden" multi-step case */
static uint8_t actual_vl[] =
{
    0x6D, 0x6D, 0x99, 0x63, 0x9E, 0x1E, 0xF3, 0x32,
    0xDD, 0xA6, 0x7F, 0x49, 0xCD, 0x60, 0xA7, 0xDC
};

/* this result came from running a single shot on this data */
static uint8_t actual_lc[] =
{
    0x0E, 0x28, 0x15, 0xCA, 0xEB, 0x3D, 0x93, 0x59,
    0x10, 0x98, 0x05, 0x62, 0x43, 0x66, 0x96, 0x2F,
    0x8E, 0x4B, 0xCA, 0x41
};

/* this result came from running a single shot on this data */
static uint8_t actual_sw[] =
{
    0xF9, 0x0B, 0x50, 0x64, 0x45, 0x00, 0xE9, 0x3E,
    0x91, 0x54, 0xF3, 0x50, 0x35, 0xB0, 0x19, 0x4A,
    0x02, 0x38, 0x6A, 0x46, 0x8D, 0xB2, 0x9C, 0x80,
    0x54, 0x9F, 0xF9, 0xD3, 0x19, 0xDD, 0xEC, 0x5B
};


typedef struct {
    uint8_t id;                   /* which string to use */
    fsl_shw_hash_alg_t algorithm; /* which algorithm to use */
    uint8_t sequence[7];          /* sequence of block sizes */
    uint8_t *result;              /* compare test results with this */
    uint8_t digest_length;        /* length of digest */
}  TEST;

static TEST test[] =
{
    {
        0,
        FSL_HASH_ALG_MD5,
        {1, 2, 0},
        actual_vl,
        16
    },
    {
        0,
        FSL_HASH_ALG_MD5,
        {2, 1, 0},
        actual_vl,
        16
    },
    {
        0,
        FSL_HASH_ALG_MD5,
        {3, 0},
        actual_vl,
        16
    },
    {
        1,
        FSL_HASH_ALG_SHA1,
        {1, 2, 0},
        actual_lc,
        20
    },
    {
        2,
        FSL_HASH_ALG_SHA256,
        {3, 2, 1, 0},
        actual_sw,
        32
    }
};


static const unsigned NUMBER_OF_TESTS = sizeof(test)/sizeof(TEST);

/* Large enough for highest value of algorithm */
static unsigned alg_supported[20];

static void setup_supported(fsl_shw_uco_t* ctx)
{
    fsl_shw_pco_t* cap = fsl_shw_get_capabilities(ctx);
    unsigned alg_count;
    unsigned i;
    fsl_shw_hash_alg_t* algs;

    for (i = 0;
         i < sizeof(alg_supported)/sizeof(unsigned);
         i++) {
        alg_supported[i] = 0;
    }

    if (cap != NULL) {
        fsl_shw_pco_get_hash_algorithms(cap, &algs, &alg_count);
        for (i = 0; i < alg_count; i++) {
            alg_supported[algs[i]] = 1;
        }
    }

    return;
}


void run_hmac2(
        fsl_shw_uco_t* my_ctx,
        uint32_t *total_passed_count,
        uint32_t *total_failed_count)
{
    uint8_t* message[3];
    uint8_t *result = (uint8_t *)malloc(MAX_CONTEXT_SIZE_BYTES);
    fsl_shw_sko_t  *key = (fsl_shw_sko_t *)malloc(sizeof(*key));
    fsl_shw_hmco_t *hco = (fsl_shw_hmco_t *)malloc(sizeof(*hco));
    fsl_shw_return_t code;
    uint8_t *msg_ptr = NULL;
    uint32_t msg_len = 0;
    uint32_t msg_segment;
    uint8_t sequence_loop;
    uint8_t test_loop;

    setup_supported(my_ctx);

    printf("HMAC Multi-step / chunking Tests\n");

    message[0] = malloc(MAX_MSG_LEN);
    message[1] = malloc(MAX_MSG_LEN);
    message[2] = malloc(MAX_MSG_LEN);

    if ((message[0] == NULL) || (message[1] == NULL) || (message[2] == NULL)
        || (result == NULL) || (key == NULL) || (hco == NULL)) {
        printf("Skipping HMAC tests (part 2) due to memory allocation"
               " failures.\n");
    } else {
        memcpy(message[0], my_msg[0], MAX_MSG_LEN);
        memcpy(message[1], my_msg[1], MAX_MSG_LEN);
        memcpy(message[2], my_msg[2], MAX_MSG_LEN);

        /* Get the precomputes */

        /* set up key information */
        fsl_shw_sko_init(key, FSL_KEY_ALG_HMAC);
        fsl_shw_sko_set_key(key, (uint8_t *)my_key, 16);

        for (test_loop = 0; test_loop < NUMBER_OF_TESTS; ++test_loop) {
            memset(result, 0, test[test_loop].digest_length);

            if (! alg_supported[test[test_loop].algorithm]) {
                printf("Skipping Precompute/One-shot HMAC Test %d:\n",
                       test_loop);
            } else {
            /* set hmac information needed for precompute */
            fsl_shw_hmco_init(hco, test[test_loop].algorithm);

            /* This function sets the INIT and PRECOMPUTES_PRESENT flags */
            code = fsl_shw_hmac_precompute(my_ctx, key, hco);

            if (code != FSL_RETURN_OK_S) {
                printf("Test %d failed: precompute not obtained: %s\n",
                       test_loop, fsl_error_string(code));
            }

            /* Chunk through bulk of message */
            if (code == FSL_RETURN_OK_S) {
                msg_ptr = message[test[test_loop].id];
                msg_len = strlen((char*)msg_ptr);
                msg_segment = 64 * test[test_loop].sequence[0];

                /* perform a one shot on this data before also chunking it */
                if (test_loop == 4) {

                    /* make it initialized (from precomputer) and finalized in
                     * one shot */
                    fsl_shw_hmco_set_flags(hco, FSL_HMAC_FLAGS_FINALIZE);
                    code = fsl_shw_hmac(my_ctx, NULL, hco, msg_ptr, msg_len,
                                        result, test[test_loop].digest_length);

                    if (code != FSL_RETURN_OK_S) {
                        printf("Precompute/One-shot HMAC Test %d failed: %s\n",
                               test_loop, fsl_error_string(code));
                        ++*total_failed_count;
                    } else {
                        if (compare_result(test[test_loop].result, result,
                                           test[test_loop].digest_length,
                                           "HMAC")) {
                            printf("Precompute/One-shot HMAC Test %d:"
                                   " Failed\n", test_loop);
                            ++*total_failed_count;
                        } else {
                            printf("Precompute/One-shot HMAC Test %d:"
                                   " Passed\n", test_loop);
                            ++*total_passed_count;
                        }
                    }
                    memset(result, 0, test[test_loop].digest_length);

                    /* reset the finalize flag */
                    fsl_shw_hmco_clear_flags(hco, FSL_HMAC_FLAGS_FINALIZE);
                }

                /* set SAVE flag (INIT already set) */
                fsl_shw_hmco_set_flags(hco, FSL_HMAC_FLAGS_SAVE);
                sequence_loop = 1;

                /* chunk thorugh the message */
                while (msg_segment != 0) {
                    code = fsl_shw_hmac(my_ctx, NULL,
                                        hco, msg_ptr, msg_segment, NULL, 0);

                    if (code != FSL_RETURN_OK_S) {
                        printf("Test %d failed: chunk %d bad: %s\n",
                               test_loop, sequence_loop,
                               fsl_error_string(code));
                        break;
                    } else {
                        /* been started, so clear INIT flag */
                        fsl_shw_hmco_clear_flags(hco, FSL_HMAC_FLAGS_INIT);
                        /* LOAD previous context (SAVE still set) */
                        fsl_shw_hmco_set_flags(hco, FSL_HMAC_FLAGS_LOAD);

                        msg_len -= msg_segment;
                        msg_ptr += msg_segment;

                        msg_segment =
                            64 * test[test_loop].sequence[sequence_loop++];

                        /* check that segment array isn't bad */
                        if (msg_segment > msg_len) {
                            msg_segment = 0;
                            msg_ptr = NULL;
                            printf("Test %d Sequence Array is bad\n",
                                   test_loop);
                        }
                    }
                }
            }

            /* finish off this message */
            if ((code == FSL_RETURN_OK_S) && (msg_ptr != NULL)) {
                /* this is the last chunk, so set FINIALIZE flag */
                fsl_shw_hmco_set_flags(hco, FSL_HMAC_FLAGS_FINALIZE);
                fsl_shw_hmco_clear_flags(hco, FSL_HMAC_FLAGS_SAVE);

                /* last chunk of message */
                code = fsl_shw_hmac(my_ctx, NULL, hco, msg_ptr,
                                    msg_len, result,
                                    test[test_loop].digest_length);

                if (code != FSL_RETURN_OK_S) {
                    printf("Test %d failed: final chunk bad: %s\n",
                           test_loop, fsl_error_string(code));
                }
            }

            if (code == FSL_RETURN_OK_S) {
                if (compare_result(test[test_loop].result, result,
                                   test[test_loop].digest_length, "HMAC")) {
                    printf("Chunking HMAC Test %d: Failed\n", test_loop);
                    ++*total_failed_count;
                } else {
                    printf("Chunking HMAC Test %d: Passed\n", test_loop);
                    ++*total_passed_count;
                }
            } else {
                ++*total_failed_count;
                printf("Chunking HMAC Test %d: Failed\n", test_loop);
            }
        }
        }
    }

    printf("\n");

    if (message[0] != NULL) {
        free(message[0]);
    }
    if (message[1] != NULL) {
        free(message[1]);
    }
    if (message[2] != NULL) {
        free(message[2]);
    }
    if (key != NULL) {
        free(key);
    }
    if (hco != NULL) {
        free(hco);
    }
    if (result != NULL) {
        free(result);
    }

    return;
}  /* run_hmac2() */

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
 * @file hash.c
 * @brief Test code for cryptographic hashing in FSL SHW API
 *
 * This file contains vectors and code to test fsl_shw_hash() and the functions
 * associated with the Hash Context Object.
 *
 * @ifnot STANDALONE
 * @ingroup MXCSAHARA2_TEST
 * @endif
 */

#include "api_tests.h"


#define MAX_CONTEXT_SIZE_BYTES 32

/**
 * This defines the breakpoint for one-shot versus multi-shot hashing.  It must
 * be a multiple of 64.
*/
#define PARTIAL_SIZE 128

#define DPD_HASH_LDCTX_HASH_ULCTX_GROUP (0x4400)

#define DPD_SHA256_LDCTX_HASH_ULCTX                                           \
             (DPD_HASH_LDCTX_HASH_ULCTX_GROUP + 0)
#define DPD_MD5_LDCTX_HASH_ULCTX                                              \
             (DPD_HASH_LDCTX_HASH_ULCTX_GROUP + 1)
#define DPD_SHA_LDCTX_HASH_ULCTX                                              \
             (DPD_HASH_LDCTX_HASH_ULCTX_GROUP + 2)
#define DPD_SHA256_LDCTX_IDGS_HASH_ULCTX                                      \
             (DPD_HASH_LDCTX_HASH_ULCTX_GROUP + 3)
#define DPD_MD5_LDCTX_IDGS_HASH_ULCTX                                         \
             (DPD_HASH_LDCTX_HASH_ULCTX_GROUP + 4)
#define DPD_SHA_LDCTX_IDGS_HASH_ULCTX                                         \
             (DPD_HASH_LDCTX_HASH_ULCTX_GROUP + 5)
#define DPD_SHA256_CONT_HASH_ULCTX                                            \
             (DPD_HASH_LDCTX_HASH_ULCTX_GROUP + 6)
#define DPD_MD5_CONT_HASH_ULCTX                                               \
             (DPD_HASH_LDCTX_HASH_ULCTX_GROUP + 7)
#define DPD_SHA_CONT_HASH_ULCTX                                               \
             (DPD_HASH_LDCTX_HASH_ULCTX_GROUP + 8)
#define DPD_SHA224_LDCTX_HASH_ULCTX                                           \
             (DPD_HASH_LDCTX_HASH_ULCTX_GROUP + 9)
#define DPD_SHA224_LDCTX_IDGS_HASH_ULCTX                                      \
             (DPD_HASH_LDCTX_HASH_ULCTX_GROUP + 10)
#define DPD_SHA224_CONT_HASH_ULCTX                                            \
             (DPD_HASH_LDCTX_HASH_ULCTX_GROUP + 12)
#define DPD_SHA256_LDCTX_HASH_ULCTX_CMP                                       \
             (DPD_HASH_LDCTX_HASH_ULCTX_GROUP + 13)
#define DPD_MD5_LDCTX_HASH_ULCTX_CMP                                          \
             (DPD_HASH_LDCTX_HASH_ULCTX_GROUP + 14)
#define DPD_SHA_LDCTX_HASH_ULCTX_CMP                                          \
             (DPD_HASH_LDCTX_HASH_ULCTX_GROUP + 15)
#define DPD_SHA256_LDCTX_IDGS_HASH_ULCTX_CMP                                  \
             (DPD_HASH_LDCTX_HASH_ULCTX_GROUP + 16)
#define DPD_MD5_LDCTX_IDGS_HASH_ULCTX_CMP                                     \
             (DPD_HASH_LDCTX_HASH_ULCTX_GROUP + 17)
#define DPD_SHA_LDCTX_IDGS_HASH_ULCTX_CMP                                     \
             (DPD_HASH_LDCTX_HASH_ULCTX_GROUP + 18)
#define DPD_SHA224_LDCTX_HASH_ULCTX_CMP                                       \
             (DPD_HASH_LDCTX_HASH_ULCTX_GROUP + 19)
#define DPD_SHA224_LDCTX_IDGS_HASH_ULCTX_CMP                                  \
             (DPD_HASH_LDCTX_HASH_ULCTX_GROUP + 20)

#define DPD_HASH_LDCTX_HASH_PAD_ULCTX_GROUP                                   \
             (0x4500)

#define DPD_SHA256_LDCTX_HASH_PAD_ULCTX                                       \
             (DPD_HASH_LDCTX_HASH_PAD_ULCTX_GROUP + 0)
#define DPD_MD5_LDCTX_HASH_PAD_ULCTX                                          \
             (DPD_HASH_LDCTX_HASH_PAD_ULCTX_GROUP + 1)
#define DPD_SHA_LDCTX_HASH_PAD_ULCTX                                          \
             (DPD_HASH_LDCTX_HASH_PAD_ULCTX_GROUP + 2)
#define DPD_SHA256_LDCTX_IDGS_HASH_PAD_ULCTX                                  \
             (DPD_HASH_LDCTX_HASH_PAD_ULCTX_GROUP + 3)
#define DPD_MD5_LDCTX_IDGS_HASH_PAD_ULCTX                                     \
             (DPD_HASH_LDCTX_HASH_PAD_ULCTX_GROUP + 4)
#define DPD_SHA_LDCTX_IDGS_HASH_PAD_ULCTX                                     \
             (DPD_HASH_LDCTX_HASH_PAD_ULCTX_GROUP + 5)
#define DPD_SHA224_LDCTX_HASH_PAD_ULCTX                                       \
             (DPD_HASH_LDCTX_HASH_PAD_ULCTX_GROUP + 6)
#define DPD_SHA224_LDCTX_IDGS_HASH_PAD_ULCTX                                  \
             (DPD_HASH_LDCTX_HASH_PAD_ULCTX_GROUP + 7)
#define DPD_SHA256_LDCTX_HASH_PAD_ULCTX_CMP                                   \
             (DPD_HASH_LDCTX_HASH_PAD_ULCTX_GROUP + 8)
#define DPD_MD5_LDCTX_HASH_PAD_ULCTX_CMP                                      \
             (DPD_HASH_LDCTX_HASH_PAD_ULCTX_GROUP + 9)
#define DPD_SHA_LDCTX_HASH_PAD_ULCTX_CMP                                      \
             (DPD_HASH_LDCTX_HASH_PAD_ULCTX_GROUP + 10)
#define DPD_SHA256_LDCTX_IDGS_HASH_PAD_ULCTX_CMP                              \
             (DPD_HASH_LDCTX_HASH_PAD_ULCTX_GROUP + 11)
#define DPD_MD5_LDCTX_IDGS_HASH_PAD_ULCTX_CMP                                 \
             (DPD_HASH_LDCTX_HASH_PAD_ULCTX_GROUP + 12)
#define DPD_SHA_LDCTX_IDGS_HASH_PAD_ULCTX_CMP                                 \
             (DPD_HASH_LDCTX_HASH_PAD_ULCTX_GROUP + 13)
#define DPD_SHA224_LDCTX_HASH_PAD_ULCTX_CMP                                   \
             (DPD_HASH_LDCTX_HASH_PAD_ULCTX_GROUP + 14)
#define DPD_SHA224_LDCTX_IDGS_HASH_PAD_ULCTX_CMP                              \
             (DPD_HASH_LDCTX_HASH_PAD_ULCTX_GROUP + 15)

typedef struct
{
    unsigned long   opId;
    unsigned long   plainTextLen;
    unsigned char   *pPlaintext;
    unsigned long   digestLen;
    unsigned char   *pDigest;
    char            testDesc[30];
} HASHTESTTYPE;

/*
 * SHA1 test vectors from FIPS PUB 180-1
 */
static const unsigned char sha1padplaintext1[] = "abc";
static const unsigned char sha1paddigest1[] =
{
    0xa9, 0x99, 0x3e, 0x36, 0x47,
    0x06, 0x81, 0x6a, 0xba, 0x3e,
    0x25, 0x71, 0x78, 0x50, 0xc2,
    0x6c, 0x9c, 0xd0, 0xd8, 0x9d
};

static const unsigned char sha1padplaintext2[] =
    "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
static const unsigned char sha1paddigest2[] =
{
    0x84, 0x98, 0x3e, 0x44, 0x1c,
    0x3b, 0xd2, 0x6e, 0xba, 0xae,
    0x4a, 0xa1, 0xf9, 0x51, 0x29,
    0xe5, 0xe5, 0x46, 0x70, 0xf1
};

/* SHA1 test vector after Sahara VL tests */
static const unsigned char sha1padplaintext3[] =
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567";
static const unsigned char sha1paddigest3[] =
{
    0xcb, 0xbc, 0xe2, 0x86, 0xc8, 0x70, 0x15, 0x60,
    0x78, 0x9f, 0x43, 0xf7, 0xb9, 0x88, 0xc9, 0xec,
    0xbb, 0x8d, 0x8d, 0xe4
};

/*
 * SHA256 test vectors from from NIST
 */
static const unsigned char sha256padplaintext1[] = "abc";
static const unsigned char sha256paddigest1[] =
{
    0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea,
    0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23,
    0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c,
    0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad
};

static const unsigned char sha256padplaintext2[] =
    "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
static const unsigned char sha256paddigest2[] =
{
    0x24, 0x8d, 0x6a, 0x61, 0xd2, 0x06, 0x38, 0xb8,
    0xe5, 0xc0, 0x26, 0x93, 0x0c, 0x3e, 0x60, 0x39,
    0xa3, 0x3c, 0xe4, 0x59, 0x64, 0xff, 0x21, 0x67,
    0xf6, 0xec, 0xed, 0xd4, 0x19, 0xdb, 0x06, 0xc1
};

/*
 * MD5 test vectors from RFC1321
 */
static const unsigned char md5padplaintext1[] = "a";
static const unsigned char md5paddigest1[] =
{
    0x0c, 0xc1, 0x75, 0xb9, 0xc0, 0xf1, 0xb6, 0xa8,
    0x31, 0xc3, 0x99, 0xe2, 0x69, 0x77, 0x26, 0x61
};

static const unsigned char md5padplaintext2[] = "abc";
static const unsigned char md5paddigest2[] =
{
    0x90, 0x01, 0x50, 0x98, 0x3c, 0xd2, 0x4f, 0xb0,
    0xd6, 0x96, 0x3f, 0x7d, 0x28, 0xe1, 0x7f, 0x72
};

static const unsigned char md5padplaintext3[] = "message digest";
static const unsigned char md5paddigest3[] =
{
    0xf9, 0x6b, 0x69, 0x7d, 0x7c, 0xb7, 0x93, 0x8d,
    0x52, 0x5a, 0x2f, 0x31, 0xaa, 0xf1, 0x61, 0xd0
};

static const unsigned char md5padplaintext4[] = "abcdefghijklmnopqrstuvwxyz";
static const unsigned char md5paddigest4[] =
{
    0xc3, 0xfc, 0xd3, 0xd7, 0x61, 0x92, 0xe4, 0x00,
    0x7d, 0xfb, 0x49, 0x6c, 0xca, 0x67, 0xe1, 0x3b
};

static const unsigned char md5padplainttext5[] =
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567";
static const unsigned char md5paddigest5[] =
{
    0xc2, 0xaa, 0xfe, 0xd0, 0x82, 0x58, 0xac, 0x2c,
    0x0e, 0xd0, 0x1e, 0x2f, 0xde, 0xde, 0x95, 0x9c
};

static const unsigned char md5nopadplaintext[] =
{
    'a', 0x80,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    8,0,0,0,0,0,0,0
};

static const unsigned char md5nopaddigest[] =
{
    0x0c, 0xc1, 0x75, 0xb9, 0xc0, 0xf1, 0xb6, 0xa8,
    0x31, 0xc3, 0x99, 0xe2, 0x69, 0x77, 0x26, 0x61
};


static const unsigned char sha1nopadplaintext[] =
{
    'a','b','c',0x80,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,24
};
static const unsigned char sha1nopaddigest[] =
{
    0xa9, 0x99, 0x3e, 0x36, 0x47,
    0x06, 0x81, 0x6a, 0xba, 0x3e,
    0x25, 0x71, 0x78, 0x50, 0xc2,
    0x6c, 0x9c, 0xd0, 0xd8, 0x9d
};

static const unsigned char sha256nopadplaintext[] =
{
    'a','b','c',0x80,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,24
};
static const unsigned char sha256nopaddigest[] =
{
    0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea,
    0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23,
    0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c,
    0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad
};


static HASHTESTTYPE hashTest[/*NUM_HASHTESTS*/] =
{
    /* SHA-1 PAD Test 1 */
    {
    DPD_SHA_LDCTX_IDGS_HASH_PAD_ULCTX,
    3,
    (unsigned char *)sha1padplaintext1,
    20,
    (unsigned char *)sha1paddigest1,
    "SHA-1 HASH PAD TEST 1"
    },

    /* SHA-1 PAD Test 2 */
    {
    DPD_SHA_LDCTX_IDGS_HASH_PAD_ULCTX,
    56,
    (unsigned char *)sha1padplaintext2,
    20,
    (unsigned char *)sha1paddigest2,
    "SHA-1 HASH PAD TEST 2"
    },

    /* SHA HASH PAD TEST 3 */
    {
    DPD_SHA_LDCTX_IDGS_HASH_PAD_ULCTX,
    1024,
    (unsigned char *)sha1padplaintext3,
    20,
    (unsigned char *)sha1paddigest3,
    "SHA-1 HASH PAD TEST 3"
    },

    /* SHA-256 PAD Test 1 */
    {
    DPD_SHA256_LDCTX_IDGS_HASH_PAD_ULCTX,
    3,
    (unsigned char *)sha256padplaintext1,
    32,
    (unsigned char *)sha256paddigest1,
    "SHA-256 HASH PAD TEST 1"
    },

    /* SHA-256 PAD Test 2 */
    {
    DPD_SHA256_LDCTX_IDGS_HASH_PAD_ULCTX,
    56,
    (unsigned char *)sha256padplaintext2,
    32,
    (unsigned char *)sha256paddigest2,
    "SHA-256 HASH PAD TEST 2"
    },

    /* MD5 PAD Test 1 */
    {
    DPD_MD5_LDCTX_IDGS_HASH_PAD_ULCTX,
    1,
    (unsigned char *)md5padplaintext1,
    16,
    (unsigned char *)md5paddigest1,
    "MD5 HASH PAD TEST 1"
    },

    /* MD5 PAD Test 2 */
    {
    DPD_MD5_LDCTX_IDGS_HASH_PAD_ULCTX,
    3,
    (unsigned char *)md5padplaintext2,
    16,
    (unsigned char *)md5paddigest2,
    "MD5 HASH PAD TEST 2"
    },

    /* MD5 PAD Test 3 */
    {
    DPD_MD5_LDCTX_IDGS_HASH_PAD_ULCTX,
    14,
    (unsigned char *)md5padplaintext3,
    16,
    (unsigned char *)md5paddigest3,
    "MD5 HASH PAD TEST 3"
    },

    /* MD5 PAD Test 4 */
    {
    DPD_MD5_LDCTX_IDGS_HASH_PAD_ULCTX,
    26,
    (unsigned char *)md5padplaintext4,
    16,
    (unsigned char *)md5paddigest4,
    "MD5 HASH PAD TEST 4"
    },

    {
    DPD_MD5_LDCTX_IDGS_HASH_PAD_ULCTX,
    1024,
    (unsigned char *)md5padplainttext5,
    16,
    (unsigned char *)md5paddigest5,
    "MD5 HASH PAD TEST 5"
    },

    /* MD5 No PAD Test */
    {
    DPD_MD5_LDCTX_IDGS_HASH_ULCTX,
    64,
    (unsigned char *)md5nopadplaintext,
    16,
    (unsigned char *)md5nopaddigest,
    "MD5 HASH No PAD TEST"
    },

    /* SHA-1 No PAD Test */
    {
    DPD_SHA_LDCTX_IDGS_HASH_ULCTX,
    64,
    (unsigned char *)sha1nopadplaintext,
    20,
    (unsigned char *)sha1nopaddigest,
    "SHA-1 HASH No PAD TEST"
    },

    /* SHA-256 No PAD Test */
    {
    DPD_SHA256_LDCTX_IDGS_HASH_ULCTX,
    64,
    (unsigned char *)sha256nopadplaintext,
    32,
    (unsigned char *)sha256nopaddigest,
    "SHA-256 HASH No PAD TEST"
    },

};

const unsigned NUM_HASHTESTS = sizeof(hashTest)/sizeof(HASHTESTTYPE);

/* returns 0 for fail, 1 for success */
static int run_hash_test(fsl_shw_uco_t* user_ctx, fsl_shw_hash_alg_t alg,
                         unsigned pad, HASHTESTTYPE* test)
{
    int failed = 1;
    fsl_shw_hco_t* hash_ctx = malloc(sizeof(*hash_ctx));
    fsl_shw_return_t code;
    uint8_t* message = malloc(test->plainTextLen);
    uint8_t* digest = malloc(test->digestLen);


    printf("\n%s\n", test->testDesc);
    if ((message == NULL) || (hash_ctx == NULL) || (digest == NULL)) {
        printf("Skipping... memory allocation failure.\n");
    } else {
        memcpy(message, test->pPlaintext, test->plainTextLen);

        memset(hash_ctx, 0x15, sizeof(*hash_ctx));

        fsl_shw_hco_init(hash_ctx, alg);
        fsl_shw_hco_set_flags(hash_ctx, FSL_HASH_FLAGS_INIT);

        memset(digest, 0, test->digestLen); /* clear out result */

        /*
         * How to run this and other multi-step tests when user ctx
         * is flagged for non-blocking??
         */
        if (test->plainTextLen > PARTIAL_SIZE) {

            fsl_shw_hco_set_flags(hash_ctx, FSL_HASH_FLAGS_SAVE);

            code = fsl_shw_hash(user_ctx, hash_ctx, message,
                                PARTIAL_SIZE, NULL, 0);

            if (code != 0) {
                printf("Test failed; first fsl_shw_hash() return %s\n",
                       fsl_error_string(code));
            } else {
                fsl_shw_hco_t* final_ctx = malloc(sizeof(*final_ctx));
                uint32_t msglen;
                uint8_t* context = malloc(MAX_CONTEXT_SIZE_BYTES);

                /* Set up new context to do rest of the hash */
                fsl_shw_hco_init(final_ctx, alg);
                fsl_shw_hco_set_flags(final_ctx, FSL_HASH_FLAGS_LOAD);
                /* Perhaps turn on SAVE as well, and extract digest/len from
                   context for comparison? */

                /* Move hash context from one object to the other. */
                fsl_shw_hco_get_digest(hash_ctx, context,
                                       MAX_CONTEXT_SIZE_BYTES, &msglen);
                fsl_shw_hco_set_digest(final_ctx, context, msglen);

                if (pad) {
                    fsl_shw_hco_set_flags(final_ctx, FSL_HASH_FLAGS_FINALIZE);
                }

                code = fsl_shw_hash(user_ctx, final_ctx,
                                    message + PARTIAL_SIZE,
                                    test->plainTextLen - PARTIAL_SIZE, digest,
                                    sizeof(digest));

                if (final_ctx != NULL) {
                    free(final_ctx);
                }
                if (context != NULL) {
                    free(context);
                }
            }
        } else {                    /* do one-shot hash */
            if (pad) {
                fsl_shw_hco_set_flags(hash_ctx, FSL_HASH_FLAGS_FINALIZE);
            }
            code = fsl_shw_hash(user_ctx, hash_ctx, message,
                                test->plainTextLen, digest, sizeof(digest));

        }

        if (code != FSL_RETURN_OK_S) {
            printf("Test failed; fsl_shw_hash() return  %s\n",
                   fsl_error_string(code));
        } else {
            if (! compare_result(test->pDigest, digest,
                                 sizeof(digest), "digest")) {
                failed = 0;
                printf("%s: Passed\n", test->testDesc);
            } else {
                printf("%s: Failed\n", test->testDesc);
            }
        }

    }

    if (message != NULL) {
        free(message);
    }

    if (hash_ctx != NULL) {
        free(hash_ctx);
    }
    if (digest != NULL) {
        free(digest);
    }

    return !failed;
}


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


/**
 * Sample code to perform Cryptographic Hashing
 *
 * @param my_ctx    User context to use
*/
void run_hash(fsl_shw_uco_t* my_ctx, uint32_t* total_passed_count,
              uint32_t* total_failed_count)
{
    unsigned testno;

    unsigned passed_count = 0;
    unsigned failed_count = 0;
    unsigned skipped_count = 0;

    setup_supported(my_ctx);

    printf("Hash: canned test count: %d\n", NUM_HASHTESTS);

    for (testno = 0; testno < NUM_HASHTESTS; testno++) {
        int passed;
        int skipped = 0;

        switch(hashTest[testno].opId) {
        case DPD_MD5_LDCTX_IDGS_HASH_PAD_ULCTX:
        case DPD_MD5_LDCTX_IDGS_HASH_PAD_ULCTX_CMP:
            if (alg_supported[FSL_HASH_ALG_MD5]) {
                passed = run_hash_test(my_ctx, FSL_HASH_ALG_MD5, 1,
                                       hashTest+testno);
            } else {
                printf("Skipping %s\n", hashTest[testno].testDesc);
                skipped = 1;
            }
            break;
        case DPD_MD5_LDCTX_IDGS_HASH_ULCTX:
        case DPD_MD5_LDCTX_IDGS_HASH_ULCTX_CMP:
            if (alg_supported[FSL_HASH_ALG_MD5]) {
                passed = run_hash_test(my_ctx, FSL_HASH_ALG_MD5, 0,
                                       hashTest+testno);
            } else {
                printf("Skipping %s\n", hashTest[testno].testDesc);
                skipped = 1;
            }
            break;
        case DPD_SHA_LDCTX_IDGS_HASH_PAD_ULCTX:
        case DPD_SHA_LDCTX_IDGS_HASH_PAD_ULCTX_CMP:
            if (alg_supported[FSL_HASH_ALG_SHA1]) {
                passed = run_hash_test(my_ctx, FSL_HASH_ALG_SHA1, 1,
                                       hashTest+testno);
            } else {
                printf("Skipping %s\n", hashTest[testno].testDesc);
                skipped = 1;
            }
            break;
        case DPD_SHA_LDCTX_IDGS_HASH_ULCTX:
        case DPD_SHA_LDCTX_IDGS_HASH_ULCTX_CMP:
            if (alg_supported[FSL_HASH_ALG_SHA1]) {
                passed = run_hash_test(my_ctx, FSL_HASH_ALG_SHA1, 0,
                                       hashTest+testno);
            } else {
                printf("Skipping %s\n", hashTest[testno].testDesc);
                skipped = 1;
            }
            break;
        case DPD_SHA224_LDCTX_IDGS_HASH_PAD_ULCTX:
        case DPD_SHA224_LDCTX_IDGS_HASH_PAD_ULCTX_CMP:
            if (alg_supported[FSL_HASH_ALG_SHA224]) {
                passed = run_hash_test(my_ctx, FSL_HASH_ALG_SHA224, 1,
                                       hashTest+testno);
            } else {
                printf("Skipping %s\n", hashTest[testno].testDesc);
                skipped = 1;
            }
            break;
        case DPD_SHA224_LDCTX_IDGS_HASH_ULCTX:
        case DPD_SHA224_LDCTX_IDGS_HASH_ULCTX_CMP:
            if (alg_supported[FSL_HASH_ALG_SHA224]) {
                passed = run_hash_test(my_ctx, FSL_HASH_ALG_SHA224, 0,
                                       hashTest+testno);
            } else {
                printf("Skipping %s\n", hashTest[testno].testDesc);
                skipped = 1;
            }
            break;
        case DPD_SHA256_LDCTX_IDGS_HASH_PAD_ULCTX:
        case DPD_SHA256_LDCTX_IDGS_HASH_PAD_ULCTX_CMP:
            if (alg_supported[FSL_HASH_ALG_SHA256]) {
                passed = run_hash_test(my_ctx, FSL_HASH_ALG_SHA256, 1,
                                       hashTest+testno);
            } else {
                printf("Skipping %s\n", hashTest[testno].testDesc);
                skipped = 1;
            }
            break;
        case DPD_SHA256_LDCTX_IDGS_HASH_ULCTX:
        case DPD_SHA256_LDCTX_IDGS_HASH_ULCTX_CMP:
            if (alg_supported[FSL_HASH_ALG_SHA256]) {
                passed = run_hash_test(my_ctx, FSL_HASH_ALG_SHA256, 0,
                                       hashTest+testno);
            } else {
                printf("Skipping %s\n", hashTest[testno].testDesc);
                skipped = 1;
            }
            break;
        default:
            printf("Unknown test type: 0x%lx\n", hashTest[testno].opId);
            passed = 0;
            continue;
        }
        if (skipped) {
            skipped_count++;
        } else if (passed) {
            passed_count++;
        } else {
            failed_count++;
        }
    }

    printf("Hash: %d tests passed, %d tests failed, %d tests skipped\n\n",
           passed_count, failed_count, skipped_count);

    *total_passed_count += passed_count;
    *total_failed_count += failed_count;

    return;
}


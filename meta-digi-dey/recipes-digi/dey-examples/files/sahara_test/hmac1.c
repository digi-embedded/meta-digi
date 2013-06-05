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
 * @file hmac1.c
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

#define MAX_CONTEXT_SIZE_BYTES 32

#define DPD_HASH_LDCTX_HMAC_ULCTX_GROUP (0x4A00)

#define DPD_SHA256_LDCTX_HMAC_ULCTX                                          \
             (DPD_HASH_LDCTX_HMAC_ULCTX_GROUP + 0)
#define DPD_MD5_LDCTX_HMAC_ULCTX                                             \
             (DPD_HASH_LDCTX_HMAC_ULCTX_GROUP + 1)
#define DPD_SHA_LDCTX_HMAC_ULCTX                                             \
             (DPD_HASH_LDCTX_HMAC_ULCTX_GROUP + 2)
#define DPD_SHA256_LDCTX_HMAC_PAD_ULCTX                                      \
             (DPD_HASH_LDCTX_HMAC_ULCTX_GROUP + 3)
#define DPD_MD5_LDCTX_HMAC_PAD_ULCTX                                         \
             (DPD_HASH_LDCTX_HMAC_ULCTX_GROUP + 4)
#define DPD_SHA_LDCTX_HMAC_PAD_ULCTX                                         \
             (DPD_HASH_LDCTX_HMAC_ULCTX_GROUP + 5)
#define DPD_SHA224_LDCTX_HMAC_ULCTX                                          \
             (DPD_HASH_LDCTX_HMAC_ULCTX_GROUP + 6)
#define DPD_SHA224_LDCTX_HMAC_PAD_ULCTX                                      \
             (DPD_HASH_LDCTX_HMAC_ULCTX_GROUP + 7)
#define DPD_SHA256_LDCTX_HMAC_ULCTX_CMP                                      \
             (DPD_HASH_LDCTX_HMAC_ULCTX_GROUP + 8)
#define DPD_MD5_LDCTX_HMAC_ULCTX_CMP                                         \
             (DPD_HASH_LDCTX_HMAC_ULCTX_GROUP + 9)
#define DPD_SHA_LDCTX_HMAC_ULCTX_CMP                                         \
             (DPD_HASH_LDCTX_HMAC_ULCTX_GROUP + 10)
#define DPD_SHA256_LDCTX_HMAC_PAD_ULCTX_CMP                                  \
             (DPD_HASH_LDCTX_HMAC_ULCTX_GROUP + 11)
#define DPD_MD5_LDCTX_HMAC_PAD_ULCTX_CMP                                     \
             (DPD_HASH_LDCTX_HMAC_ULCTX_GROUP + 12)
#define DPD_SHA_LDCTX_HMAC_PAD_ULCTX_CMP                                     \
             (DPD_HASH_LDCTX_HMAC_ULCTX_GROUP + 13)
#define DPD_SHA224_LDCTX_HMAC_ULCTX_CMP                                      \
             (DPD_HASH_LDCTX_HMAC_ULCTX_GROUP + 14)
#define DPD_SHA224_LDCTX_HMAC_PAD_ULCTX_CMP                                  \
             (DPD_HASH_LDCTX_HMAC_ULCTX_GROUP + 15)

typedef struct
{
    unsigned long  opId;
    uint32_t       flags; /* hmac flags */
    unsigned long key_length;
    const unsigned char *key;
    unsigned long  message_length;
    const unsigned char *message;
    unsigned long  digest_length; /* digest length, or 0 */
    const unsigned char *digest;
    unsigned long  context_length; /* opad/ipad lengths, or 0 */
    const unsigned char  *opad;
    const unsigned char  *ipad;
    char          testDesc[34];
} HMACTESTTYPE;

static const unsigned char sha1testkey1[] =
{
    0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
    0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
    0x0B, 0x0B, 0x0B, 0x0B
};

static const unsigned char sha1testmessage1[] =
{
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x31, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x32, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x33, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x34, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x35, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x36, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x37, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37
};

static const unsigned char sha1testdigest1[] =
{
    0xFB, 0x6D, 0xCB, 0xAF, 0x78, 0x92, 0x6E, 0x99,
    0xFB, 0x13, 0xB0, 0x14, 0x28, 0x6E, 0xA3, 0xFA,
    0xFA, 0x65, 0x9B, 0xE1
};

static const unsigned char sha1testipad1[] =
{
    0x06, 0x4C, 0x66, 0x2A, 0xCC, 0x6E, 0xD1, 0xCC,
    0x6C, 0xFA, 0x9A, 0xB0, 0x04, 0x2F, 0x13, 0x7F,
    0xCE, 0xA5, 0x70, 0xEB, 0x40, 0x00, 0x00, 0x00
};

static const unsigned char sha1testopad1[] =
{
    0xD6, 0x2A, 0xCC, 0xCC, 0x7E, 0x6A, 0x98, 0xB3,
    0xDF, 0x01, 0x5B, 0x02, 0xD8, 0x85, 0xC3, 0xE0,
    0x9B, 0xD1, 0x84, 0xCB, 0x40, 0x00, 0x00, 0x00
};

static const unsigned char md5testkey1[] =
{
    0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
    0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
    0x0B, 0x0B, 0x0B, 0x0B
};

static const unsigned char md5testmessage1[] =
{
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x31, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x32, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x33, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x34, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x35, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x36, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x37, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37
};

static const unsigned char md5testdigest1[] =
{
    0x79, 0xD3, 0x12, 0x94, 0x76, 0xC6, 0x67, 0xB5,
    0xD2, 0x4B, 0x05, 0x32, 0x0F, 0x85, 0x24, 0xE0
};

static const unsigned char md5testipad1[] =
{
    0x2A, 0x2F, 0x00, 0x9E, 0xFB, 0x9C, 0x79, 0xD2,
    0x1B, 0xE7, 0xDB, 0xE8, 0x64, 0xEA, 0xC8, 0xA9,
    0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00
};

static const unsigned char md5testopad1[] =
{
    0xC2, 0xC2, 0xA3, 0x9B, 0xD8, 0x1C, 0x6B, 0xBD,
    0x74, 0x33, 0x4B, 0x4D, 0xBD, 0xBD, 0x21, 0xF0,
    0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00
};

static const unsigned char sha256testkey1[] =
{
    0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
    0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
    0x0B, 0x0B, 0x0B, 0x0B
};

static const unsigned char sha256testmessage1[] =
{
    0x48, 0x69, 0x20, 0x54, 0x68, 0x65, 0x72, 0x65
};

static const unsigned char sha256testdigest1[] =
{
    0xB0, 0x34, 0x4C, 0x61, 0xD8, 0xDB, 0x38, 0x53,
    0x5C, 0xA8, 0xAF, 0xCE, 0xAF, 0x0B, 0xF1, 0x2B,
    0x88, 0x1D, 0xC2, 0x00, 0xC9, 0x83, 0x3D, 0xA7,
    0x26, 0xE9, 0x37, 0x6C, 0x2E, 0x32, 0xCF, 0xF7
};

static const unsigned char sha256testipad1[] =
{
    0x2B, 0xB2, 0x18, 0x04, 0x23, 0xB9, 0x5B, 0xF9,
    0xB4, 0xE8, 0x25, 0x8C, 0xFA, 0xB5, 0xE6, 0x54,
    0x11, 0xF2, 0x92, 0x1E, 0x4F, 0xEB, 0x78, 0xEE,
    0x98, 0x90, 0xE5, 0xFE, 0x64, 0xB7, 0x80, 0x36,
    0x40, 0x00, 0x00, 0x00
};

static const unsigned char sha256testopad1[] =
{
    0x27, 0xE7, 0x73, 0x9F, 0xD9, 0x56, 0x25, 0x83,
    0x56, 0xD6, 0x66, 0xE2, 0x5F, 0x81, 0x0D, 0xE8,
    0xEC, 0x5E, 0x4F, 0x8A, 0x55, 0x3D, 0x4F, 0xB8,
    0x3C, 0xFF, 0x20, 0xBA, 0x10, 0x23, 0x4B, 0x40,
    0x40, 0x00, 0x00, 0x00
};

/*
 * HMAC-MD5 test vector from RFC2202
 */

static const unsigned char md5padkeydata1[] = "Jefe"; /* 4 */
/* Plaintext padded manually for no pad test */
static const unsigned char md5padplaintext1[] = /* 64 */
{
    'w','h','a','t',' ','d','o',' ',
    'y','a',' ','w','a','n','t',' ',
    'f','o','r',' ','n','o','t','h',
    'i','n','g','?',0x80,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0xe0,0x02,0,0,0,0,0,0
};
static const unsigned char md5paddigest1[] = /* 16 */
{
    0x75, 0x0c, 0x78, 0x3e, 0x6a, 0xb0, 0xb5, 0x03,
    0xea, 0xa8, 0x6e, 0x31, 0x0a, 0x5d, 0xb7, 0x38
};

/*
 * HMAC-SHA1 test vector from RFC2202
 */
static const unsigned char sha1padkeydata1[] = "Jefe"; /* 4 */
/* Plaintext padded manually for no pad test */
static const unsigned char sha1padplaintext1[] =  /* 64 */
{
    'w','h','a','t',' ','d','o',' ',
    'y','a',' ','w','a','n','t',' ',
    'f','o','r',' ','n','o','t','h',
    'i','n','g','?',0x80,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0x02,0xe0
};
static const unsigned char sha1paddigest1[] = /* 20 */
{
    0xef, 0xfc, 0xdf, 0x6a, 0xe5, 0xeb, 0x2f, 0xa2, 0xd2, 0x74,
    0x16, 0xd5, 0xf1, 0x84, 0xdf, 0x9c, 0x25, 0x9a, 0x7c, 0x79
};

/*
 * HMAC-SHA256 test vector from
 * draft-ietf-ipsec-ciph-sha-256-01.txt
 */
static const unsigned char sha256padkeydata1[] = /* 32 */
{
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
    0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
    0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20
};
/* Plaintext padded manually for no pad test */
static const unsigned char sha256padplaintext1[] = /* 64 */
{
    'a','b','c',0x80,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0x02,0x18
};
static const unsigned char sha256paddigest1[] = /* 32 */
{
    0xa2, 0x1b, 0x1f, 0x5d, 0x4c, 0xf4, 0xf7, 0x3a,
    0x4d, 0xd9, 0x39, 0x75, 0x0f, 0x7a, 0x06, 0x6a,
    0x7f, 0x98, 0xcc, 0x13, 0x1c, 0xb1, 0x6a, 0x66,
    0x92, 0x75, 0x90, 0x21, 0xcf, 0xab, 0x81, 0x81
};


static const HMACTESTTYPE hmactests[] =
{
    /* test digest can be generated starting with precomputes */
    {   /* SHA-1 HMAC 64 20byte*/
        DPD_SHA_LDCTX_HMAC_PAD_ULCTX,
        FSL_HMAC_FLAGS_FINALIZE, /* hmac flags */
        20,                     /* key len */
        sha1testkey1,
        64,                     /* msg len */
        sha1testmessage1,
        20,                     /* digest len */
        sha1testdigest1,
        24,                     /* pad len */
        sha1testipad1,
        sha1testopad1,
        "SHA-1 HMAC 64 20byte Flags IFP"
    },
    {   /* MD5 HMAC 64 20byte*/
        DPD_MD5_LDCTX_HMAC_PAD_ULCTX,
        FSL_HMAC_FLAGS_FINALIZE, /* hmac flags */
        20,                     /* key len */
        md5testkey1,
        64,                     /* msg len */
        md5testmessage1,
        16,                     /* digest len */
        md5testdigest1,
        24,                     /* pad len */
        md5testipad1,
        md5testopad1,
        "MD5 HMAC 64 20byte Flags IFP"
    },
    {   /* SHA-256 HMAC 64 20byte*/
        DPD_SHA256_LDCTX_HMAC_PAD_ULCTX,
        FSL_HMAC_FLAGS_FINALIZE, /* hmac flags */
        20,                     /* key len */
        sha256testkey1,
        8,                     /* msg len */
        sha256testmessage1,
        32,                     /* digest len */
        sha256testdigest1,
        36,                     /* pad len */
        sha256testipad1,
        sha256testopad1,
        "SHA-256 HMAC 64 20byte Flags IFP"
    },

    /* test that adding the save flag does not hurt the results */
    /* (note: this can be used in white box testing by setting the output
     * pointers of the descriptors to result instead of hmac_ctx) */
    {   /* SHA-1 HMAC 64 20byte*/
        DPD_SHA_LDCTX_HMAC_PAD_ULCTX,
        FSL_HMAC_FLAGS_FINALIZE | FSL_HMAC_FLAGS_SAVE, /* hmac flags */
        20,                     /* key len */
        sha1testkey1,
        64,                     /* msg len */
        sha1testmessage1,
        20,                     /* digest len */
        sha1testdigest1,
        24,                     /* pad len */
        sha1testipad1,
        sha1testopad1,
        "SHA-1 HMAC 64 20byte Flags ISFP"
    },
    {   /* MD5 HMAC 64 20byte*/
        DPD_MD5_LDCTX_HMAC_PAD_ULCTX,
        FSL_HMAC_FLAGS_FINALIZE | FSL_HMAC_FLAGS_SAVE, /* hmac flags */
        20,                     /* key len */
        md5testkey1,
        64,                     /* msg len */
        md5testmessage1,
        16,                     /* digest len */
        md5testdigest1,
        24,                     /* pad len */
        md5testipad1,
        md5testopad1,
        "MD5 HMAC 64 20byte Flags ISFP"
    },
    {   /* SHA-256 HMAC 64 20byte*/
        DPD_SHA256_LDCTX_HMAC_PAD_ULCTX,
        FSL_HMAC_FLAGS_FINALIZE | FSL_HMAC_FLAGS_SAVE, /* hmac flags */
        20,                     /* key len */
        sha256testkey1,
        8,                     /* msg len */
        sha256testmessage1,
        32,                     /* digest len */
        sha256testdigest1,
        36,                     /* pad len */
        sha256testipad1,
        sha256testopad1,
        "SHA-256 HMAC 64 20byte Flags ISFP"
    },

    /* test digest can be generated starting with the key */
    {   /* MD5 HMAC PAD Test 1 */
        DPD_MD5_LDCTX_HMAC_PAD_ULCTX,
        FSL_HMAC_FLAGS_FINALIZE | FSL_HMAC_FLAGS_INIT, /* hmac flags */
        4,
        (unsigned char *)md5padkeydata1,
        28,
        (unsigned char *)md5padplaintext1,
        16,
        (unsigned char *)md5paddigest1,
        0,
        NULL,
        NULL,
        "MD5 HMAC PAD TEST 1 Flags IF"
    },
    {   /* SHA-1 HMAC PAD Test 1 */
        DPD_SHA_LDCTX_HMAC_PAD_ULCTX,
        FSL_HMAC_FLAGS_FINALIZE | FSL_HMAC_FLAGS_INIT, /* hmac flags */
        4,
        (unsigned char *)sha1padkeydata1,
        28,
        (unsigned char *)sha1padplaintext1,
        20,
        (unsigned char *)sha1paddigest1,
        0,
        NULL,
        NULL,
        "SHA-1 HMAC PAD TEST 1 Flags IF"
    },
    {   /* SHA-256 HMAC PAD Test 1 */
        DPD_SHA256_LDCTX_HMAC_PAD_ULCTX,
        FSL_HMAC_FLAGS_FINALIZE | FSL_HMAC_FLAGS_INIT, /* hmac flags */
        32,
        (unsigned char *)sha256padkeydata1,
        3,
        (unsigned char *)sha256padplaintext1,
        32,
        (unsigned char *)sha256paddigest1,
        0,
        NULL,
        NULL,
        "SHA-256 HMAC PAD TEST 1 Flags IF"
    },

    /* test that adding the save flag does not hurt the results */
    {   /* MD5 HMAC PAD Test 1 */
        DPD_MD5_LDCTX_HMAC_PAD_ULCTX,
        FSL_HMAC_FLAGS_FINALIZE | FSL_HMAC_FLAGS_INIT | FSL_HMAC_FLAGS_SAVE,
        4,
        (unsigned char *)md5padkeydata1,
        28,
        (unsigned char *)md5padplaintext1,
        16,
        (unsigned char *)md5paddigest1,
        0,
        NULL,
        NULL,
        "MD5 HMAC PAD TEST 1 Flags ISF"
    },
    {   /* SHA-1 HMAC PAD Test 1 */
        DPD_SHA_LDCTX_HMAC_PAD_ULCTX,
        FSL_HMAC_FLAGS_FINALIZE | FSL_HMAC_FLAGS_INIT | FSL_HMAC_FLAGS_SAVE,
        4,
        (unsigned char *)sha1padkeydata1,
        28,
        (unsigned char *)sha1padplaintext1,
        20,
        (unsigned char *)sha1paddigest1,
        0,
        NULL,
        NULL,
        "SHA-1 HMAC PAD TEST 1 Flags ISF"
    },
    {   /* SHA-256 HMAC PAD Test 1 */
        DPD_SHA256_LDCTX_HMAC_PAD_ULCTX,
        FSL_HMAC_FLAGS_FINALIZE | FSL_HMAC_FLAGS_INIT | FSL_HMAC_FLAGS_SAVE,
        32,
        (unsigned char *)sha256padkeydata1,
        3,
        (unsigned char *)sha256padplaintext1,
        32,
        (unsigned char *)sha256paddigest1,
        0,
        NULL,
        NULL,
        "SHA-256 HMAC PAD TEST 1 Flags ISF"
#if 0
    },
    {   /* MD5 HMAC No PAD Test 1 */
        DPD_MD5_LDCTX_HMAC_ULCTX,
        FSL_HMAC_FLAGS_FINALIZE, /* hmac flags */
        4,
        (unsigned char *)md5padkeydata1,
        64,
        (unsigned char *)md5padplaintext1,
        16,
        (unsigned char *)md5paddigest1,
        0,
        NULL,
        NULL,
        "MD5 HMAC NO PAD TEST 1"
    },
    {   /* SHA-1 HMAC No PAD Test 1 */
        DPD_SHA_LDCTX_HMAC_ULCTX,
        FSL_HMAC_FLAGS_FINALIZE, /* hmac flags */
        4,
        (unsigned char *)sha1padkeydata1,
        64,
        (unsigned char *)sha1padplaintext1,
        20,
        (unsigned char *)sha1paddigest1,
        0,
        NULL,
        NULL,
        "SHA-1 HMAC NO PAD TEST 1"
    },
    {   /* SHA-256 HMAC No PAD Test 1 */
        DPD_SHA256_LDCTX_HMAC_ULCTX,
        FSL_HMAC_FLAGS_FINALIZE, /* hmac flags */
        32,
        (unsigned char *)sha256padkeydata1,
        64,
        (unsigned char *)sha256padplaintext1,
        32,
        (unsigned char *)sha256paddigest1,
        0,
        NULL,
        NULL,
        "SHA-256 HMAC NO PAD TEST 1"
#endif
    }
};


static const unsigned NUM_HMACTESTS = sizeof(hmactests)/sizeof(HMACTESTTYPE);


static int run_hmac_test(fsl_shw_uco_t* my_ctx, fsl_shw_hash_alg_t algorithm,
                         const HMACTESTTYPE* test);


static int run_hmac_test(
    fsl_shw_uco_t* my_ctx,
    fsl_shw_hash_alg_t algorithm,
    const HMACTESTTYPE* test)
{
    /* The completed MAC - large enough for anything */
    uint8_t* message = malloc(test->message_length);
    unsigned char* hmac = malloc(MAX_CONTEXT_SIZE_BYTES);
    fsl_shw_hmco_t* hmac_ctx = malloc(sizeof(*hmac_ctx));
    fsl_shw_sko_t  key_info; /* for the HMAC */
    fsl_shw_return_t code;
    unsigned passed = 0;

    printf("Starting %s:\n", test->testDesc);

    if ((hmac == NULL) || (hmac_ctx == NULL)) {
        printf("Skipping due to memory allocation failures.\n");
    } else {
        memcpy(message, test->message, test->message_length);

        memset(&key_info, 0x8a, sizeof(key_info));
        memset(hmac_ctx, 0x4f, sizeof(*hmac_ctx));

        /* Initialize the objects */
        fsl_shw_hmco_init(hmac_ctx, algorithm);

        fsl_shw_sko_init(&key_info, FSL_KEY_ALG_HMAC);

        /* Store key in the key object */
        fsl_shw_sko_set_key(&key_info, test->key, test->key_length);

        /* Do pre-compute or not... */
        if (test->context_length != 0) {
            /* Calculate the IPAD and OPAD from the key */
            code = fsl_shw_hmac_precompute(my_ctx, &key_info, hmac_ctx);
            if (code != FSL_RETURN_OK_S) {
                printf("Test failed: fsl_shw_hmac_precompute() returned "
                       "%d %s\n", code, fsl_error_string(code));
            } else {
                memset(hmac, 0, MAX_CONTEXT_SIZE_BYTES);
                /* Finish the HMAC by running the message through */
                /* INIT and PRECOMPUTES_PRESENT are set in precompute call */
                fsl_shw_hmco_set_flags(hmac_ctx, test->flags);

                code = fsl_shw_hmac(my_ctx, &key_info, hmac_ctx, message,
                                    test->message_length, hmac,
                                    test->digest_length);
                if (code != FSL_RETURN_OK_S) {
                    printf("Test failed: fsl_shw_hmac() returned %d %s\n",
                           code, fsl_error_string(code));
                }
            }
        } else {
            /* No pre-compute */
            memset(hmac, 0, MAX_CONTEXT_SIZE_BYTES);
            /* Set up for one-shot HMAC of all data... */
            fsl_shw_hmco_set_flags(hmac_ctx, test->flags);
            /* Compute HMAC from start to finish */
            code = fsl_shw_hmac(my_ctx, &key_info, hmac_ctx, message,
                                test->message_length, hmac,
                                test->digest_length);
            if (code != FSL_RETURN_OK_S) {
                printf("Test failed: fsl_shw_hmac() returned %d %s\n",
                       code, fsl_error_string(code));
            }
        }

        /* Verify HMAC */
        if (code == FSL_RETURN_OK_S) {
            if (!compare_result(test->digest, hmac, test->digest_length,
                                "HMAC")) {
                passed = 1;
            }
        }
    }

    if (message != NULL) {
        free(message);
    }
    if (hmac != NULL) {
        free(hmac);
    }
    if (hmac_ctx != NULL) {
        free(hmac_ctx);
    }

    return passed;
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
 * Sample code to calculate an HMAC in one operation
 *
 * @param my_ctx    User context to use
*/
void run_hmac1(fsl_shw_uco_t* my_ctx, uint32_t* total_passed_count,
               uint32_t* total_failed_count)
{
    unsigned testno;

    unsigned passed_count = 0;
    unsigned failed_count = 0;
    unsigned skipped_count = 0;
    int skipped = 0;
    int passed;

    setup_supported(my_ctx);

    printf("canned HMAC test count: %d\n", NUM_HMACTESTS);

    for (testno = 0; testno < NUM_HMACTESTS; testno++) {
        switch (hmactests[testno].opId)
        {
        case DPD_SHA224_LDCTX_HMAC_ULCTX:
        case DPD_SHA224_LDCTX_HMAC_ULCTX_CMP:
        case DPD_SHA256_LDCTX_HMAC_ULCTX:
        case DPD_SHA256_LDCTX_HMAC_ULCTX_CMP:
        case DPD_SHA_LDCTX_HMAC_ULCTX:
        case DPD_SHA_LDCTX_HMAC_ULCTX_CMP:
        case DPD_MD5_LDCTX_HMAC_ULCTX:
        case DPD_MD5_LDCTX_HMAC_ULCTX_CMP:
            printf("Skipping test %s - User Prepad is not supported\n",
                   hmactests[testno].testDesc);
            continue;
            break;

        case DPD_SHA224_LDCTX_HMAC_PAD_ULCTX:
        case DPD_SHA224_LDCTX_HMAC_PAD_ULCTX_CMP:
            if (alg_supported[FSL_HASH_ALG_SHA224]) {
                passed = run_hmac_test(my_ctx, FSL_HASH_ALG_SHA224,
                                       hmactests + testno);
            } else {
                skipped = 1;
            }
            break;

        case DPD_SHA256_LDCTX_HMAC_PAD_ULCTX:
        case DPD_SHA256_LDCTX_HMAC_PAD_ULCTX_CMP:
            if (alg_supported[FSL_HASH_ALG_SHA256]) {
                passed = run_hmac_test(my_ctx, FSL_HASH_ALG_SHA256,
                                       hmactests + testno);
            } else {
                skipped = 1;
            }
            break;

        case DPD_SHA_LDCTX_HMAC_PAD_ULCTX:
        case DPD_SHA_LDCTX_HMAC_PAD_ULCTX_CMP:
            if (alg_supported[FSL_HASH_ALG_SHA1]) {
                passed = run_hmac_test(my_ctx, FSL_HASH_ALG_SHA1,
                                       hmactests + testno);
            } else {
                skipped = 1;
            }
            break;

        case DPD_MD5_LDCTX_HMAC_PAD_ULCTX:
        case DPD_MD5_LDCTX_HMAC_PAD_ULCTX_CMP:
            if (alg_supported[FSL_HASH_ALG_MD5]) {
                passed = run_hmac_test(my_ctx, FSL_HASH_ALG_MD5,
                                       hmactests + testno);
            } else {
                skipped = 1;
            }
            break;

        default:
            printf("Unknown test type: %lu\n", hmactests[testno].opId);
            continue;
            break;
        } /* switch */

        if (skipped) {
            printf("%s: Skipped\n\n", hmactests[testno].testDesc);
            skipped_count++;
        } else if (passed) {
            printf("%s: Passed\n\n", hmactests[testno].testDesc);
            passed_count++;
        } else {
            printf("%s: Failed\n\n", hmactests[testno].testDesc);
            failed_count++;
    }

    } /* for testno ... */

    printf("HMAC: %d tests passed, %d tests failed, %d tests skipped\n\n",
           passed_count, failed_count, skipped_count);

    *total_passed_count += passed_count;
    *total_failed_count += failed_count;

    return;
}

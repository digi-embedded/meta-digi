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
 * @file cap.c
 * @brief Test code for retrieving hardware capabilities
 *
 * @ifnot STANDALONE
 * @ingroup MXCSAHARA2_TEST
 * @endif
 */

#include "api_tests.h"


struct interpret
{
    unsigned symbol;
    char*    name;
};

static struct interpret keyalgs[] =
{
    {FSL_KEY_ALG_AES, "AES"},
    {FSL_KEY_ALG_DES, "DES"},
    {FSL_KEY_ALG_TDES, "3DES"},
    {FSL_KEY_ALG_ARC4, "ARC4"}
};

static struct interpret symmodes[] =
{
    {FSL_SYM_MODE_STREAM, "Stream"},
    {FSL_SYM_MODE_ECB, "ECB"},
    {FSL_SYM_MODE_CBC, "CBC"},
    {FSL_SYM_MODE_CTR, "CTR"}
};

static struct interpret hashalgs[] =
{
    {FSL_HASH_ALG_MD5, "MD5"},
    {FSL_HASH_ALG_SHA1, "SHA1"},
    {FSL_HASH_ALG_SHA224, "SHA224"},
    {FSL_HASH_ALG_SHA256, "SHA256"}
};


/**
 * Sample code to query the Platform Capabilities Object
 *
 * @param my_ctx    User context to use
 */
void show_capabilities(fsl_shw_uco_t* my_ctx, uint32_t* total_passed_count,
                       uint32_t* total_failed_count)
{
    fsl_shw_pco_t* cap = fsl_shw_get_capabilities(my_ctx);
    fsl_shw_key_alg_t* capkeyalg;
    fsl_shw_sym_mode_t* capsymmode;
    fsl_shw_hash_alg_t* caphashalg;
    unsigned count;
    unsigned i;
    unsigned j;

    if (cap == NULL) {
        printf("Get capabilities failed!\n");
    } else {
        int major;
        int minor;


        printf("\nVersion numbers:");

        fsl_shw_pco_get_version(cap, &major, &minor);
        printf("\n   API %d.%d", major, minor);

        fsl_shw_pco_get_driver_version(cap, &major, &minor);
        printf("\n   Driver %d.%d", major, minor);

        printf("\nSymmetric algorithms:");
        fsl_shw_pco_get_sym_algorithms(cap, &capkeyalg, &count);
        for (i = 0; i < count; i++) {
            for (j = 0; j < sizeof(keyalgs)/sizeof(struct interpret); j++) {
                if (capkeyalg[i] == keyalgs[j].symbol) {
                    printf(" %s", keyalgs[j].name);
                    break;
                }
            }
        }

        printf("\nSymmetric modes:");
        fsl_shw_pco_get_sym_modes(cap, &capsymmode, &count);
        for (i = 0; i < count; i++) {
            for (j = 0; j < sizeof(symmodes)/sizeof(struct interpret); j++) {
                if (capsymmode[i] == symmodes[j].symbol) {
                    printf(" %s", symmodes[j].name);
                    break;
                }
            }
        }

        for (j = 0; j < sizeof(keyalgs)/sizeof(struct interpret); j++) {
            printf("\n   Modes supported for %s: ",  keyalgs[j].name);
            for (i = 0; i < sizeof(symmodes)/sizeof(struct interpret); i++) {
                if (fsl_shw_pco_check_sym_supported(cap, keyalgs[j].symbol,
                                                    symmodes[i].symbol)) {
                    printf("%s ", symmodes[i].name);
                };
            }
        }

        printf("\nHash algorithms:");
        fsl_shw_pco_get_hash_algorithms(cap, &caphashalg, &count);
        for (i = 0; i < count; i++) {
            for (j = 0; j < sizeof(hashalgs)/sizeof(struct interpret); j++) {
                if (caphashalg[i] == hashalgs[j].symbol) {
                    printf(" %s", hashalgs[j].name);
                    break;
                }
            }
        }
    }

    /* Do not report pass/fail.  Zero for 'unused param' warning. */
    total_passed_count = 0;
    total_failed_count = 0;

    printf("\n\n");
}

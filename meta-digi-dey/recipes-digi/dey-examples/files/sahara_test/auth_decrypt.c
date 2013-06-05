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
 * @file auth_decrypt.c
 *
 * @brief Test code to verify an authentication code and decrypt data.
 *
 * Testing is performed in gen_encrypt.c
 */

#include "api_tests.h"

/**
 * Test fsl_shw_auth_decrypt().
 *
 * @param my_ctx    User context to use
 */
void run_auth_decrypt(fsl_shw_uco_t* my_ctx, uint32_t* total_passed_count,
                      uint32_t* total_failed_count)
{
    my_ctx = 0;
    total_passed_count = 0;
    total_failed_count = 0;

    return;
}

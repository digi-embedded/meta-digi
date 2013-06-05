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

#include <fsl_shw.h>

#include "apihelp.h"

#ifndef __KERNEL__
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#else

#define usleep os_mdelay
#endif

void run_tests(fsl_shw_uco_t *, const char *test_string, uint32_t * passed,
	       uint32_t * failed);
void run_auth_decrypt(fsl_shw_uco_t *, uint32_t * passed, uint32_t * failed);
void run_gen_encrypt(fsl_shw_uco_t *, uint32_t * passed, uint32_t * failed);
void run_hash(fsl_shw_uco_t *, uint32_t * passed, uint32_t * failed);
void run_hmac1(fsl_shw_uco_t *, uint32_t * passed, uint32_t * failed);
void run_hmac2(fsl_shw_uco_t *, uint32_t * passed, uint32_t * failed);
void run_pkha(fsl_shw_uco_t *, uint32_t * passed, uint32_t * failed);
void run_random(fsl_shw_uco_t *, uint32_t * passed, uint32_t * failed);
void run_result(fsl_shw_uco_t *, uint32_t * passed, uint32_t * failed);
void show_capabilities(fsl_shw_uco_t *, uint32_t * passed, uint32_t * failed);
void run_symmetric(fsl_shw_uco_t *, uint32_t * passed, uint32_t * failed);
void run_wrap(fsl_shw_uco_t *, uint32_t * passed, uint32_t * failed);
void run_user_wrap(fsl_shw_uco_t *, uint32_t * passed, uint32_t * failed);
void run_callback(fsl_shw_uco_t *, uint32_t * passed, uint32_t * failed);
void run_smalloc(fsl_shw_uco_t *, uint32_t * passed, uint32_t * failed);
void run_dryice(fsl_shw_uco_t *, uint32_t * passed, uint32_t * failed);

/*
 * Copyright 2009 Freescale Semiconductor, Inc. All rights reserved.
 */

/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

#include "api_tests.h"

#ifndef __KERNEL__
#include <signal.h>
#include <setjmp.h>

jmp_buf safe_state;		/* Hold a stack state */
struct sigaction catch_error;	/* Define the error trapping callback */
struct sigaction catch_default;	/* Place to store the original state */

/* Signal trap to allow testing illegal operations on the partition */
void catch_signal(int sig)
{
	(void)sig;
	printf("in catch_signal\n");
	longjmp(safe_state, 1);
}

/* set up the sigaction structure so it can be used for error trapping */
void catch_signal_init()
{
	catch_error.sa_handler = catch_signal;
	sigemptyset(&catch_error.sa_mask);
	catch_error.sa_flags = 0;
}

#define TRY                                                                 \
    if (setjmp(safe_state) == 0) {                                           \
        sigaction(SIGSEGV, &catch_error, &catch_default);

#define CATCH                                                               \
    } else {

#define DONETRY                                                             \
    }                                                                       \
    sigaction(SIGSEGV, &catch_default, NULL);

#endif				/* __KERNEL__ */

static uint8_t secret_data[64] =
    "All mimsy were the borogoves... All mimsy were the borogoves...";
#define secret_data_len sizeof(secret_data)

static int test_encrypt_decrypt(fsl_shw_uco_t * my_ctx, uint32_t partition_size)
{
	uint32_t *partition_base;
	int passed = 0;

	uint32_t permissions =
	    FSL_PERM_TH_R | FSL_PERM_TH_W |
	    FSL_PERM_HD_R | FSL_PERM_HD_W | FSL_PERM_HD_X |
	    FSL_PERM_OT_R | FSL_PERM_OT_W | FSL_PERM_OT_X;
	uint8_t UMID[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	uint32_t IV[4] = { 0x12345678, 0, 0, 0 };
	uint8_t buff[64];

	printf("Attempting to grab a secure partition:\n");

	partition_base =
	    fsl_shw_smalloc(my_ctx, partition_size, UMID, permissions);

	if (partition_base == NULL) {
		printf("Skipping...  failed to get a secure partition.\n");
		goto out;
	}

	/* load data onto partition */
	memcpy(partition_base, secret_data, secret_data_len);

	/* do encrypt */
	do_scc_encrypt_region(my_ctx, partition_base, 0,
			      secret_data_len, buff,
			      IV, FSL_SHW_CYPHER_MODE_CBC);

	/* do decrypt */
	do_scc_decrypt_region(my_ctx, partition_base, secret_data_len,
			      secret_data_len, buff,
			      IV, FSL_SHW_CYPHER_MODE_CBC);

	/* compare */
	if (memcmp((void *)partition_base,
		   (void *)partition_base + secret_data_len,
		   secret_data_len) == 0) {
		passed = 1;
		printf("Encrypt/Decrypt region tests passed.\n");
	} else {
		printf("Encrypt/Decrypt region tests failed.\n");
	}

      out:
	if (partition_base != NULL) {
		fsl_shw_sfree(my_ctx, partition_base);
	}

	return passed;
}

#ifndef __KERNEL__

static int test_user_permissions(fsl_shw_uco_t * my_ctx,
				 uint32_t partition_size)
{
	uint32_t *partition_base;
	uint32_t *partition_base_copy;
	uint32_t i, test;
	int passed = 0;

	uint32_t permissions =
	    FSL_PERM_TH_R | FSL_PERM_TH_W |
	    FSL_PERM_HD_R | FSL_PERM_HD_W | FSL_PERM_HD_X |
	    FSL_PERM_OT_R | FSL_PERM_OT_W | FSL_PERM_OT_X;
	uint8_t UMID[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	catch_signal_init();

	printf("Testing read/write permissions across the partition:\n");

	partition_base =
	    fsl_shw_smalloc(my_ctx, partition_size, UMID, permissions);

	partition_base_copy = partition_base;

	if (partition_base == NULL) {
		printf("Skipping...  failed to get a secure partition.\n");
		goto out;
	}

	/* Tests that should pass */
	for (test = 1; test < 3; test++) {
		TRY {
			switch (test) {
			case 1:
				printf
				    (" Part %i: Write across whole partition\n",
				     test);
				for (i = 0; i < (partition_size / 4); i++) {
					partition_base[i] = i;
				}
				break;
			case 2:
				printf
				    (" Part %i: Read across whole partition\n",
				     test);
				for (i = 0; i < (partition_size / 4); i++) {
					if (partition_base[i] != i) {
						printf
						    ("\n reading failed at position %i.  Expected: %i, Read: %i\n",
						     i, i, partition_base[i]);
					}
				}
				break;
			}
		}
		CATCH {
			printf(" failed during passing part %i\n", test);
			goto out;
		}
		DONETRY printf(" passed part %i\n", test);
	}

#if 0
	/* Tests that should fail */
	for (test = 1; test < 3; test++) {
		printf("trying round two\n");
		TRY {
			switch (test) {
			case 1:
				printf
				    (" Part %i: Write outside of partition bounds\n",
				     test);
				partition_base[partition_size / 4] = 0xFFFFFFFF;
				break;
			case 2:
				printf
				    (" Part %i: Read outside of partition bounds\n",
				     test);
				i = partition_base[partition_size / 4];
				break;

				printf(" failed during failing part %i\n",
				       test);
				goto out;
			}
		}
		CATCH {
			/* Failure here is expected */
			printf("failed part %i, good\n", test);
		}
	DONETRY}
	count++;
#endif

	passed = 1;

      out:
	if (partition_base != NULL) {
		/* NOTE: the sfree method itself should also be checked, in case it is
		 * implemented incorrectly.
		 */
		fsl_shw_sfree(my_ctx, partition_base);
	}

	return passed;
}

#endif				/* __KERNEL__ */

/*!
 *  Test the secure partition interface by:
 *
 * - Testing to see if the platform supports secure memory, then allocating
 *   a partition.
 * - Read and write across the entire partition
 * - Encrypt and decrypt a region on the partition using the secret key.
 * - (not enabled) Attempting to read/write outside of the allocated memory
 *
 * @param my_ctx    User context to use
 */
void run_smalloc(fsl_shw_uco_t * my_ctx, uint32_t * total_passed_count,
		 uint32_t * total_failed_count)
{
	fsl_shw_pco_t *capabilities = fsl_shw_get_capabilities(my_ctx);
	uint32_t partition_size;

	/* First test to see if the platform supports secure memory. */
	if ((capabilities == NULL) ||
	    (!fsl_shw_pco_check_spo_supported(capabilities))) {
		printf("Skipping Secure Memory tests\n\n");
	} else {
		printf("\nTest: SMALLOC\n");

		partition_size = fsl_shw_pco_get_spo_size_bytes(capabilities);

#ifndef __KERNEL__

		if (test_user_permissions(my_ctx, partition_size)) {
			*total_passed_count += 1;
		} else {
			*total_failed_count += 1;
		}

#endif

		if (test_encrypt_decrypt(my_ctx, partition_size)) {
			*total_passed_count += 1;
		} else {
			*total_failed_count += 1;
		}
	}
}

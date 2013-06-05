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

/*!
 * @file dryice.c
 * @brief Test code for DryIce support in FSL SHW API
 *
 * @ifnot STANDALONE
 * @ingroup MXCSAHARA2_TEST
 * @endif
 */

#include "api_tests.h"

static uint8_t input_data[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
};

static uint8_t wrap_unwrap_key_a[] = {
	0x01, 0x23, 0x45, 0x67,
	0x01, 0x23, 0x45, 0x67,
	0x76, 0x54, 0x32, 0x10,
	0x76, 0x54, 0x32, 0x10,
	0x08, 0x09, 0x0a, 0x0b,
	0x0c, 0x0d, 0x0e, 0x0f
};

static uint8_t wrap_unwrap_input_a[] = {
	0x00, 0x01, 0x02, 0x03,
	0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0a, 0x0b,
	0x0c, 0x0d, 0x0e, 0x0f
};

static uint8_t wrap_unwrap_output_a[] = {
	0x1A, 0xB0, 0xA5, 0x21,
	0xDA, 0x31, 0x3B, 0xD3,
	0xE4, 0xB3, 0xFE, 0xB1,
	0x37, 0x5A, 0xC9, 0xE1
};

static uint8_t wrap_unwrap_key_b[] = {
	0x76, 0x54, 0x32, 0x10,
	0x76, 0x54, 0x32, 0x10,
	0x01, 0x23, 0x45, 0x67,
	0x01, 0x23, 0x45, 0x67
};

static uint8_t wrap_unwrap_input_b[] = {
	0x00, 0x01, 0x02, 0x03,
	0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0a, 0x0b,
	0x0c, 0x0d, 0x0e, 0x0f
};

static uint8_t wrap_unwrap_output_b[] = {
	0xD5, 0x00, 0x22, 0x0E,
	0x06, 0xF8, 0xAB, 0xC1,
	0x50, 0x64, 0x33, 0x29,
	0x10, 0x8C, 0xAC, 0x69
};

/*!
 * Test that PK wrap/unwrap functions work correctly
 *
 * First encrypt some data with a known hardware key, then wrap the key.
 * Next, load a different known key and encrypt some data with it, to ensure
 * that the first key has been properly overwritten. Finally, unwrap the
 * original key and encrypt some data again to verify that it has been restored
 * properly.
 */
static void test_wrap_unwrap(fsl_shw_uco_t * my_ctx,
			     uint32_t * total_passed_count,
			     uint32_t * total_failed_count)
{
	fsl_shw_sko_t program_key_a;
	fsl_shw_sko_t program_key_b;
	fsl_shw_sko_t program_key_a_unwrapped;
	fsl_shw_scco_t ctx;
	fsl_shw_return_t ret;
	uint8_t output_a[sizeof(wrap_unwrap_input_a)];
	uint8_t output_b[sizeof(wrap_unwrap_input_b)];
	uint8_t output_c[sizeof(wrap_unwrap_input_a)];
	uint8_t *wrapped_key_data = NULL;
	int wrapped_key_size;
	int passed = 0;
	int failed = 0;

	/* Initial control object */
	fsl_shw_scco_init(&ctx, FSL_KEY_ALG_TDES, FSL_SYM_MODE_ECB);

	/* Establish a known (plaintext) program key */
	fsl_shw_sko_init_pf_key(&program_key_a, FSL_KEY_ALG_TDES,
				FSL_SHW_PF_KEY_PRG);

	fsl_shw_sko_set_key_length(&program_key_a, sizeof(wrap_unwrap_key_a));
	fsl_shw_sko_set_user_id(&program_key_a, 0x123456);
	ret = fsl_shw_establish_key(my_ctx, &program_key_a, FSL_KEY_WRAP_ACCEPT,
				    wrap_unwrap_key_a);
	if (ret != FSL_RETURN_OK_S) {
		printf
		    ("First establish program key from plaintext failed with %s\n",
		     fsl_error_string(ret));
		failed++;
		goto out;
	}

	/* Perform a known cipher operation with the key to ensure it functions
	 * correctly.
	 */
	ret = fsl_shw_symmetric_encrypt(my_ctx, &program_key_a, &ctx,
					sizeof(wrap_unwrap_input_a),
					wrap_unwrap_input_a, output_a);
	if (ret != FSL_RETURN_OK_S) {
		printf
		    ("First symmetric encrypt in wrap/unwrap test failed with %s\n",
		     fsl_error_string(ret));
		failed++;
		goto out;
	}
	if (compare_result(wrap_unwrap_output_a, output_a,
			   sizeof(wrap_unwrap_output_a), "third enc output")) {
		failed++;
		goto out;
	}

	/* Wrap the program key, and release it. */
	fsl_shw_sko_calculate_wrapped_size(&program_key_a, &wrapped_key_size);
	wrapped_key_data = malloc(wrapped_key_size + 1);
	if (wrapped_key_data == NULL) {
		printf("Failed to allocate memory to store wrapped key\n");
		failed++;
		goto out;
	}

	wrapped_key_data[wrapped_key_size] = 0x42;
	fsl_shw_extract_key(my_ctx, &program_key_a, wrapped_key_data);
	if (ret != FSL_RETURN_OK_S) {
		printf("Extract program key failed with %s\n",
		       fsl_error_string(ret));
		failed++;
		goto out;
	}

	if (wrapped_key_data[wrapped_key_size] != 0x42) {
		printf("Wrapped key buffer was overwritten\n");
		failed++;
		goto out;
	}

	/* Release the program key */
	fsl_shw_release_key(my_ctx, &program_key_a);
	if (ret != FSL_RETURN_OK_S) {
		printf
		    ("First release (hardware) key in wrap/unwrap failed with %s\n",
		     fsl_error_string(ret));
		failed++;
		goto out;
	}

	/* Establish a second known program key */
	fsl_shw_sko_init_pf_key(&program_key_b, FSL_KEY_ALG_TDES,
				FSL_SHW_PF_KEY_PRG);

	fsl_shw_sko_set_key_length(&program_key_b, sizeof(wrap_unwrap_key_b));
	fsl_shw_establish_key(my_ctx, &program_key_b, FSL_KEY_WRAP_ACCEPT,
			      wrap_unwrap_key_b);
	if (ret != FSL_RETURN_OK_S) {
		printf
		    ("Second establish program key from plaintext failed with %s\n",
		     fsl_error_string(ret));
		failed++;
		goto out;
	}

	/* Perform a known cipher operation with the key to ensure it functions
	 * correctly.
	 */
	ret = fsl_shw_symmetric_encrypt(my_ctx, &program_key_b, &ctx,
					sizeof(wrap_unwrap_input_b),
					wrap_unwrap_input_b, output_b);

	if (ret != FSL_RETURN_OK_S) {
		printf
		    ("Second symmetric encrypt in wrap/unwrap test failed with %s\n",
		     fsl_error_string(ret));
		failed++;
		goto out;
	}
	if (compare_result(wrap_unwrap_output_b, output_b,
			   sizeof(wrap_unwrap_output_b), "second enc output")) {
		failed++;
		goto out;
	}

	/* Release the program key */
	fsl_shw_release_key(my_ctx, &program_key_b);
	if (ret != FSL_RETURN_OK_S) {
		printf
		    ("Second release (hardware) key in wrap/unwrap failed with %s\n",
		     fsl_error_string(ret));
		failed++;
		goto out;
	}

	/* Establish (unwrap) the original program key */
	fsl_shw_sko_init_pf_key(&program_key_a_unwrapped, FSL_KEY_ALG_TDES,
				FSL_SHW_PF_KEY_PRG);

	fsl_shw_sko_set_key_length(&program_key_a_unwrapped,
				   sizeof(wrap_unwrap_key_a));
	fsl_shw_sko_set_user_id(&program_key_a_unwrapped, 0x123456);
	fsl_shw_establish_key(my_ctx, &program_key_a_unwrapped,
			      FSL_KEY_WRAP_UNWRAP, wrapped_key_data);

	/* Perform a known cipher operation with the key to ensure that it was
	 * unwrapped correctly.
	 */
	ret = fsl_shw_symmetric_encrypt(my_ctx, &program_key_a_unwrapped, &ctx,
					sizeof(wrap_unwrap_input_a),
					wrap_unwrap_input_a, output_c);

	if (ret != FSL_RETURN_OK_S) {
		printf
		    ("Third symmetric encrypt in wrap/unwrap test failed with %s\n",
		     fsl_error_string(ret));
		failed++;
		goto out;
	}
	if (compare_result(wrap_unwrap_output_a, output_c,
			   sizeof(wrap_unwrap_output_a), "third enc output")) {
		failed++;
		goto out;
	}

	/* Release the program key */
	fsl_shw_release_key(my_ctx, &program_key_a_unwrapped);
	if (ret != FSL_RETURN_OK_S) {
		printf
		    ("Third release (hardware) key in wrap/unwrap failed with %s\n",
		     fsl_error_string(ret));
		failed++;
		goto out;
	}

	passed++;
	printf("Hardware wrap/unwrap Passed\n");
      out:
	if (wrapped_key_data != NULL) {
		free(wrapped_key_data);
	}

	*total_passed_count += passed;
	*total_failed_count += failed;
	return;
}				/* end fn test_wrap_unwrap */

/*!
 * Make sure that HW random key gets generated
 *
 * First encrypt some data with the random key, then regenerate the random key,
 * then encrypt the same data again.  The encrypted results should differ.
 */
static void test_pf_random(fsl_shw_uco_t * my_ctx,
			   uint32_t * total_passed_count,
			   uint32_t * total_failed_count)
{
	fsl_shw_return_t ret;
	fsl_shw_sko_t rnd_key;
	fsl_shw_scco_t ctx;
	uint8_t output1[sizeof(input_data)];
	uint8_t output2[sizeof(input_data)];
	int passed = 0;
	int failed = 0;

	/* Get a reference to the pf random key */
	fsl_shw_sko_init_pf_key(&rnd_key, FSL_KEY_ALG_TDES,
				FSL_SHW_PF_KEY_IIM_RND);

	/* Initial control object */
	fsl_shw_scco_init(&ctx, FSL_KEY_ALG_TDES, FSL_SYM_MODE_ECB);

	/* Encrypt with the current random key */
	ret = fsl_shw_symmetric_encrypt(my_ctx, &rnd_key, &ctx,
					sizeof(input_data), input_data,
					output1);
	if (ret != FSL_RETURN_OK_S) {
		printf
		    ("First symmetric encrypt in HW random test failed with %s\n",
		     fsl_error_string(ret));
		failed++;
		goto out;
	}

	/* Set up a new random key */
	if (0) {
		printf("SKIPPING generation of HW random key\n");
	} else {
		ret = fsl_shw_gen_random_pf_key(my_ctx);
		if (ret != FSL_RETURN_OK_S) {
			printf("Error generating HW Random Key: %s\n",
			       fsl_error_string(ret));
			failed++;
			goto out;
		}
		printf("Finished generation of HW random key\n");
	}

	/* Encrypt with the new random key */
	ret = fsl_shw_symmetric_encrypt(my_ctx, &rnd_key, &ctx,
					sizeof(input_data), input_data,
					output2);
	if (ret != FSL_RETURN_OK_S) {
		printf
		    ("Second symmetric encrypt in HW random test failed with %s\n",
		     fsl_error_string(ret));
		failed++;
		goto out;
	}

	if (memcmp(output1, output2, sizeof(input_data)) == 0) {
		printf("Random HW key did not appear to change\n");
		failed++;
		goto out;
	}

	passed++;
	printf("Random HW key test passed\n");

      out:
	*total_passed_count += passed;
	*total_failed_count += failed;
	return;
}				/* end fn test_pf_random */

/*!
 * Make sure no error or events are returned from read_tamper_event()
 */
static void test_tamper(fsl_shw_uco_t * my_ctx, uint32_t * total_passed_count,
			uint32_t * total_failed_count)
{
	fsl_shw_return_t ret;
	fsl_shw_tamper_t event = (fsl_shw_tamper_t) - 1;
	uint64_t timestamp = 0;
	int passed = 0;
	int failed = 0;

	/* Can we force a tamper event to appear??? */

	ret = fsl_shw_read_tamper_event(my_ctx, &event, &timestamp);
	if (ret != FSL_RETURN_OK_S) {
		printf("fsl_shw_read_tamper_event returned with error: %s\n",
		       fsl_error_string(ret));
		failed++;
		goto out;
	}

	if (event != FSL_SHW_TAMPER_NONE) {
		printf("What the hey?  A tamper event?  %d / %llu\n", event,
		       (unsigned long long)timestamp);
		failed++;
		goto out;
	}

	printf("Tamper Passed:  No events\n");
	passed++;

      out:
	*total_passed_count += passed;
	*total_failed_count += failed;
	return;
}				/* end fn test_tamper */

/*!
 * Entry point for dryice test module.
 *
 * Responsible for running other tests, as appropriate.
 */
void run_dryice(fsl_shw_uco_t * my_ctx, uint32_t * total_passed_count,
		uint32_t * total_failed_count)
{
	fsl_shw_pco_t *cap = fsl_shw_get_capabilities(my_ctx);

	if ((cap == NULL)) {
		printf("DryIce - Cap is null \n\n");
	}
	if (!fsl_shw_pco_check_pk_supported(cap)) {
		printf("Skipping DryIced Tests\n\n");
		goto out;
	}

	test_tamper(my_ctx, total_passed_count, total_failed_count);

	test_pf_random(my_ctx, total_passed_count, total_failed_count);

	test_wrap_unwrap(my_ctx, total_passed_count, total_failed_count);

      out:
	return;
}

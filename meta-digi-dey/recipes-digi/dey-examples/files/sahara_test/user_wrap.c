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
 * @file user_wrap.c
 * @brief Test code for Wrapped (Black) Key support in FSL SHW API
 *
 * This file contains vectors and code to test fsl_shw_establish_key(),
 * fsl_shw_extract_key(), fsl_shw_release_key(), and the functions associated
 * with the Secret Key Object.
 *
 * @ifnot STANDALONE
 * @ingroup MXCSAHARA2_TEST
 * @endif
 */

#include "api_tests.h"

/*! Key for Known Answer test */
static const unsigned char known_key[] = {
	0x2C, 0x82, 0x96, 0xE0, 0x2E, 0x5F, 0x5C, 0x19,
	0xAA, 0x29, 0xA6, 0xCF, 0x97, 0x05, 0x5C, 0xD2,
	0xA8, 0xEC, 0xE4, 0x1D, 0xAC, 0x47, 0x7B, 0x6F
};

/*! Plaintext for Known Answer test */
static unsigned char known_plaintext[] = {
	0x0F, 0xEB, 0x9B, 0x5C, 0x22, 0x0B, 0xA5, 0x13,
	0x5D, 0x0F, 0x55, 0x06, 0xC7, 0xD6, 0x75, 0xAF,
	0x76, 0x20, 0x1A, 0x91, 0x78, 0x31, 0x75, 0x94,
	0x67, 0xB7, 0x3D, 0x23, 0x90, 0xEC, 0x4E, 0x4F,
	0x84, 0x55, 0xB0, 0xED, 0x4B, 0x81, 0x70, 0x85,
	0x1D, 0xB1, 0xD5, 0x48, 0x85, 0x7A, 0x13, 0x40,
	0x76, 0x74, 0x7C, 0x92, 0x97, 0x75, 0xB3, 0x14,
	0xA7, 0xE5, 0x02, 0x4F, 0xB4, 0x2F, 0x1E, 0x03
};

/*! Result of encrypting Known plaintext with 24-byte TDES key in ECB mode. */
static const unsigned char TDES_known_ciphertext[] = {
	0x79, 0xA2, 0xCB, 0x79, 0x43, 0x66, 0x07, 0x61,
	0xB6, 0x53, 0xEF, 0x8B, 0xD9, 0x6C, 0xD6, 0x45,
	0xF4, 0xEF, 0x29, 0x4F, 0x37, 0x07, 0x5F, 0x2B,
	0xBE, 0xE9, 0x5E, 0x6A, 0x27, 0x29, 0x02, 0x45,
	0x22, 0x7F, 0x80, 0x16, 0xFF, 0x21, 0x83, 0xB4,
	0x19, 0x80, 0xED, 0x66, 0x37, 0x28, 0x8E, 0xA0,
	0xEA, 0x7F, 0x3A, 0x96, 0x37, 0xE2, 0x07, 0x24,
	0x87, 0x42, 0x35, 0xCB, 0x11, 0xDE, 0xEB, 0xC3
};

/*! Result of encrypting Known plaintext with 15-byte ARC4 key. */
static const unsigned char ARC4_known_ciphertext[] = {
	0x38, 0xeb, 0x67, 0x0a, 0x6b, 0x26, 0x2b, 0x6f,
	0x40, 0x15, 0x42, 0x04, 0x2f, 0xee, 0x48, 0x9a,
	0x78, 0x75, 0x22, 0xba, 0x70, 0x20, 0xec, 0x78,
	0x89, 0x60, 0x3c, 0xd9, 0xf9, 0x97, 0x6e, 0x0f,
	0x2f, 0xad, 0xe0, 0x02, 0xc5, 0x7e, 0xfd, 0xc3,
	0x3a, 0x93, 0x74, 0xb3, 0x5f, 0xdb, 0xd0, 0xdf,
	0xe2, 0xbc, 0x7c, 0x24, 0xa3, 0xe0, 0xde, 0x78,
	0x11, 0x08, 0x25, 0x02, 0xae, 0xea, 0x04, 0x17
};

#define KEY_OWNER_ID 0x42

#define SECRET_KEY_SIZE 7
#define SECRET_KEY_ALGORITHM FSL_KEY_ALG_ARC4

#define KNOWN_ARC4_KEY_SIZE 15
#define KNOWN_ARC4_KEY_ALGORITHM FSL_KEY_ALG_ARC4

#define KNOWN_TDES_KEY_SIZE 24
#define KNOWN_TDES_KEY_ALGORITHM FSL_KEY_ALG_TDES

/*!
 * Helper function to set values in Secret Key and
 * Symmetric Cipher Context objects according to the
 * key algorithm and key length passed in.
 */
static void init_key_and_sym_ctx(fsl_shw_key_alg_t key_alg,
				 unsigned key_len,
				 fsl_shw_sko_t * key_info,
				 fsl_shw_scco_t * sym_ctx)
{
	fsl_shw_sko_init(key_info, key_alg);
	fsl_shw_sko_set_key_length(key_info, key_len);

	if ((key_alg == FSL_KEY_ALG_TDES) || (key_alg == FSL_KEY_ALG_DES)) {
		fsl_shw_sko_set_flags(key_info, FSL_SKO_KEY_IGNORE_PARITY);
		fsl_shw_scco_init(sym_ctx, key_alg, FSL_SYM_MODE_ECB);
	} else {
		fsl_shw_scco_init(sym_ctx, key_alg, FSL_SYM_MODE_STREAM);
	}

	fsl_shw_scco_set_flags(sym_ctx, FSL_SYM_CTX_INIT);

	return;
}

/*!
 * Establish a generated key and then verify that encryptiong and
 * decryption both work on it.  As a side effect, leave encrypted
 * value in the calling argument.
 */
static int create_key(fsl_shw_uco_t * my_ctx, fsl_shw_sko_t * key_info,
		      fsl_shw_scco_t * sym_ctx, const uint8_t * plaintext,
		      uint32_t len, uint8_t * encrypt_output)
{
	int passed = 0;
	fsl_shw_return_t code;
	uint8_t *decrypt_output = malloc(2 * len);

	if (decrypt_output != NULL) {
		code =
		    fsl_shw_establish_key(my_ctx, key_info, FSL_KEY_WRAP_CREATE,
					  NULL);
		if (code != FSL_RETURN_OK_S) {
			printf("fsl_shw_establish_key(CREATE) returned: %s\n",
			       fsl_error_string(code));
		} else {
			memset(encrypt_output, 0, len);
			/* Encrypt an arbitrary vector.  Try to decrypt to same value. */
			code =
			    fsl_shw_symmetric_encrypt(my_ctx, key_info, sym_ctx,
						      len, plaintext,
						      encrypt_output);
			if (code != FSL_RETURN_OK_S) {
				printf
				    ("fsl_shw_symmetric_encrypt() returned: %s\n",
				     fsl_error_string(code));
			} else {
				memset(decrypt_output, 0, len);
				code =
				    fsl_shw_symmetric_decrypt(my_ctx, key_info,
							      sym_ctx, len,
							      encrypt_output,
							      decrypt_output);
				if (code != FSL_RETURN_OK_S) {
					printf
					    ("fsl_shw_symmetric_decrypt() returned: %s\n",
					     fsl_error_string(code));
				} else {
					if (!compare_result
					    (plaintext, decrypt_output, len,
					     "decrypted plaintext")) {
						passed = 1;
					}
				}
			}
		}
	}

	/* Clean up any allocated memory */
	if (decrypt_output) {
		free(decrypt_output);
	}

	return passed;
}

/*!
 * Wrap a key and then unwrap it.  Use it to decrypt a message, then verify
 * that the message matches the expected message.
 */
static int extract_reestablish_key(fsl_shw_uco_t * my_ctx,
				   fsl_shw_kso_t * my_keystore,
				   uint32_t ownerid, uint32_t handle,
				   fsl_shw_scco_t * sym_ctx,
				   const uint8_t * ciphertext,
				   const uint8_t * expected_plaintext,
				   uint32_t len)
{
	int passed = 0;
	uint8_t *blob = NULL;
	uint32_t blob_length;
	fsl_shw_sko_t old_key_info;	/* existing, established key info */
	fsl_shw_sko_t new_key_info;	/* to-be-unwrapped key info */
	fsl_shw_return_t code;
	uint32_t key_len = SECRET_KEY_SIZE;
	fsl_shw_key_alg_t key_alg = SECRET_KEY_ALGORITHM;
	uint8_t *decrypt_output = malloc(2 * len);

	init_key_and_sym_ctx(key_alg, key_len, &old_key_info, sym_ctx);

	fsl_shw_sko_set_established_info(&old_key_info, ownerid, handle);
	fsl_shw_sko_set_keystore(&old_key_info, my_keystore);

	fsl_shw_sko_calculate_wrapped_size(&old_key_info, &blob_length);
	blob = malloc(blob_length);

	if (decrypt_output != NULL) {
		memset(blob, 0, blob_length);
		code = fsl_shw_extract_key(my_ctx, &old_key_info, blob);
		if (code != FSL_RETURN_OK_S) {
			fsl_shw_return_t err_err;

			printf("fsl_shw_extract_key() returned: %s\n",
			       fsl_error_string(code));
			err_err = fsl_shw_release_key(my_ctx, &old_key_info);
			if (err_err != 0) {
				printf
				    ("Warning: could not release key with handle 0x%x: %s\n",
				     handle, fsl_error_string(err_err));
			}
		} else {
			init_key_and_sym_ctx(key_alg, key_len, &new_key_info,
					     sym_ctx);

			fsl_shw_sko_set_user_id(&new_key_info, KEY_OWNER_ID);
			fsl_shw_sko_set_keystore(&new_key_info, my_keystore);

			code = fsl_shw_establish_key(my_ctx, &new_key_info,
						     FSL_KEY_WRAP_UNWRAP, blob);

			if (code != FSL_RETURN_OK_S) {
				printf
				    ("fsl_shw_establish_key(UNWRAP) returned: %s\n",
				     fsl_error_string(code));
			} else {
				fsl_shw_return_t err_err;

				/* Try to decrypt what had been done with previous
				   incarnation. */
				memset(decrypt_output, 0, len);
				code =
				    fsl_shw_symmetric_decrypt(my_ctx,
							      &new_key_info,
							      sym_ctx, len,
							      ciphertext,
							      decrypt_output);
				if (code != FSL_RETURN_OK_S) {
					printf
					    ("fsl_shw_symmetric_decrypt() returned: %s\n",
					     fsl_error_string(code));
				} else {
					if (!compare_result
					    (expected_plaintext, decrypt_output,
					     len, "decrypted plaintext")) {
						passed = 1;
					}
				}

				err_err =
				    fsl_shw_release_key(my_ctx, &new_key_info);
				if (err_err != 0) {
					printf
					    ("Warning: could not release key with handle 0x%x: "
					     "%s\n", handle,
					     fsl_error_string(err_err));
				}
			}
		}
	}

	/* Clean up any allocated memory */
	if (decrypt_output) {
		free(decrypt_output);
	}
	if (blob) {
		free(blob);
	}

	return passed;
}

/*!
 * Test Key-Wrapping routines.
 */
void run_user_wrap(fsl_shw_uco_t * my_ctx, uint32_t * total_passed_count,
		   uint32_t * total_failed_count)
{
	fsl_shw_kso_t keystore;
	fsl_shw_sko_t key_info;
	fsl_shw_scco_t *sym_ctx = malloc(sizeof(*sym_ctx));
	fsl_shw_return_t code;
	uint8_t *encrypt_input = malloc(sizeof(known_plaintext));
	uint8_t *decrypt_input = malloc(sizeof(known_plaintext));
	uint8_t *encrypt_output = malloc(2 * sizeof(known_plaintext));
	uint8_t *decrypt_output = malloc(2 * sizeof(known_plaintext));
	uint8_t *blob = NULL;
	uint32_t blob_length;
	int passed_count = 0;
	int failed_count = 0;
	uint32_t handle;
	int passed = 0;
	int testno = 1;
	int i;
	fsl_shw_pco_t *cap = fsl_shw_get_capabilities(my_ctx);

	fsl_shw_init_keystore_default(&keystore);

	if ((cap == NULL) || !fsl_shw_pco_check_black_key_supported(cap)
	    || !fsl_shw_pco_check_spo_supported(cap)) {
		printf("Skipping User Keystore Wrapped / Black Key Tests\n\n");
	} else if ((encrypt_output == NULL) || (decrypt_output == NULL)
		   || (encrypt_input == NULL) || (decrypt_input == NULL)
		   || (sym_ctx == NULL)) {
		printf
		    ("Memory allocation problems. Skipping user wrapped Key Tests\n");
		*total_failed_count += 6;
	} else if (fsl_shw_establish_keystore(my_ctx, &keystore)
		   != FSL_RETURN_OK_S) {
		printf
		    ("Failed to establish user keystore.  Skipping user wrapped Key"
		     " Tests.\n");
		*total_failed_count += 6;
	} else {
		uint32_t key_len = SECRET_KEY_SIZE;
		fsl_shw_key_alg_t key_alg = SECRET_KEY_ALGORITHM;

		memcpy(encrypt_input, known_plaintext, sizeof(known_plaintext));

		/* for test, fill with garbage */
		memset(&key_info, 0x2d, sizeof(key_info));
		memset(sym_ctx, 0x51, sizeof(*sym_ctx));

		init_key_and_sym_ctx(key_alg, key_len, &key_info, sym_ctx);
		fsl_shw_sko_set_user_id(&key_info, KEY_OWNER_ID);
		fsl_shw_sko_set_keystore(&key_info, &keystore);

		printf
		    ("Secret Key Test %d: Generate and use a random %d-byte RED"
		     " key\n", testno, key_len);

		passed = create_key(my_ctx, &key_info, sym_ctx,
				    encrypt_input, sizeof(known_plaintext),
				    encrypt_output);

		printf("Secret Key Test %d: %s\n\n", testno++,
		       passed ? "passed" : "failed");
		if (passed) {
			passed_count++;
		} else {
			failed_count++;
		}

		printf("Secret Key Test %d: Extract and Re-establish RED key\n",
		       testno);

		fsl_shw_sko_get_established_info(&key_info, &handle);
		passed =
		    extract_reestablish_key(my_ctx, &keystore, KEY_OWNER_ID,
					    handle, sym_ctx, encrypt_output,
					    encrypt_input,
					    sizeof(known_plaintext));

		printf("Secret Key Test %d: %s\n\n", testno++,
		       passed ? "passed" : "failed");
		if (passed) {
			passed_count++;
		} else {
			failed_count++;
		}

		/* Now run tests with Known Key values of two different lengths. */

		/* First time through is with ARC4 key */
		key_len = KNOWN_ARC4_KEY_SIZE;
		key_alg = KNOWN_ARC4_KEY_ALGORITHM;
		for (i = 0; i < 2; i++) {

			/* fill with garbage */
			memset(&key_info, 0x2c, sizeof(key_info));
			memset(sym_ctx, 0x51, sizeof(sym_ctx));

			init_key_and_sym_ctx(key_alg, key_len, &key_info,
					     sym_ctx);
			fsl_shw_sko_set_user_id(&key_info, KEY_OWNER_ID);
			fsl_shw_sko_set_keystore(&key_info, &keystore);

			if (key_alg == FSL_KEY_ALG_TDES) {
				memcpy(decrypt_input, TDES_known_ciphertext,
				       sizeof(known_plaintext));
			} else {
				memcpy(decrypt_input, ARC4_known_ciphertext,
				       sizeof(known_plaintext));
			}

			printf
			    ("Secret Key Test %d: Establish a known %u-byte RED key\n",
			     testno, key_len);
			passed = 0;

			code = fsl_shw_establish_key(my_ctx, &key_info,
						     FSL_KEY_WRAP_ACCEPT,
						     known_key);

			if (code != FSL_RETURN_OK_S) {
				printf
				    ("fsl_shw_establish_key(ACCEPT) returned: %s\n",
				     fsl_error_string(code));
			} else {
				memset(encrypt_output, 0,
				       sizeof(known_plaintext));
				code =
				    fsl_shw_symmetric_encrypt(my_ctx, &key_info,
							      sym_ctx,
							      sizeof
							      (known_plaintext),
							      encrypt_input,
							      encrypt_output);
				if (code != FSL_RETURN_OK_S) {
					printf
					    ("fsl_shw_symmetric_encrypt() returned: %s\n",
					     fsl_error_string(code));
				} else {
					if (!compare_result
					    (decrypt_input, encrypt_output,
					     sizeof(known_plaintext),
					     "encrypted ciphertext")) {
						passed = 1;
					}
				}
			}

			printf("Secret Key Test %d: %s\n\n", testno++,
			       passed ? "passed" : "failed");
			if (passed) {
				passed_count++;
			} else {
				failed_count++;
			}

			printf
			    ("Secret Key Test %d: Re-establish known %u-byte RED key\n",
			     testno, key_len);
			passed = 0;

			fsl_shw_sko_calculate_wrapped_size(&key_info,
							   &blob_length);
			blob = malloc(blob_length);
			if (blob == NULL) {
				printf("Allocation failed; aborting test\n");
			} else {

				code =
				    fsl_shw_extract_key(my_ctx, &key_info,
							blob);
				if (code != FSL_RETURN_OK_S) {
					fsl_shw_return_t err_err;
					printf
					    ("fsl_shw_extract_key() returned: %s\n",
					     fsl_error_string(code));
					fsl_shw_sko_get_established_info
					    (&key_info, &handle);
					err_err =
					    fsl_shw_release_key(my_ctx,
								&key_info);
					if (err_err != 0) {
						printf
						    ("Warning: could not release key with handle "
						     "0x%x: %s\n", handle,
						     fsl_error_string(err_err));
					}
				} else {
					if (key_alg == FSL_KEY_ALG_ARC4) {
						fsl_shw_scco_init(sym_ctx,
								  key_alg,
								  FSL_SYM_MODE_STREAM);
						fsl_shw_scco_set_flags(sym_ctx,
								       FSL_SYM_CTX_INIT);
					} else {
						if ((key_alg ==
						     FSL_KEY_ALG_TDES)
						    || (key_alg ==
							FSL_KEY_ALG_DES)) {
							fsl_shw_sko_set_flags
							    (&key_info,
							     FSL_SKO_KEY_IGNORE_PARITY);
						}
						fsl_shw_scco_init(sym_ctx,
								  key_alg,
								  FSL_SYM_MODE_ECB);
					}

					fsl_shw_sko_set_user_id(&key_info,
								KEY_OWNER_ID);
					fsl_shw_sko_set_keystore(&key_info,
								 &keystore);

					code =
					    fsl_shw_establish_key(my_ctx,
								  &key_info,
								  FSL_KEY_WRAP_UNWRAP,
								  blob);
					if (code != FSL_RETURN_OK_S) {
						printf
						    ("fsl_shw_establish_key(UNWRAP) returned: %s\n",
						     fsl_error_string(code));
					} else {
						/* Try to decrypt what had been done with previous
						   incarnation. */
						memset(decrypt_output, 0,
						       sizeof(known_plaintext));
						code =
						    fsl_shw_symmetric_decrypt
						    (my_ctx, &key_info, sym_ctx,
						     sizeof(known_plaintext),
						     decrypt_input,
						     decrypt_output);
						if (code != FSL_RETURN_OK_S) {
							printf
							    ("fsl_shw_symmetric_decrypt() returned:"
							     " %s\n",
							     fsl_error_string
							     (code));
						} else {
							if (!compare_result
							    (known_plaintext,
							     decrypt_output,
							     sizeof
							     (known_plaintext),
							     "decrypted plaintext"))
							{
								passed = 1;
							}
						}
						fsl_shw_release_key(my_ctx,
								    &key_info);
					}
				}
			}

			printf("Secret Key Test %d: %s\n\n", testno++,
			       passed ? "passed" : "failed");
			if (passed) {
				passed_count++;
			} else {
				failed_count++;
			}

			/* Next time through use TDES key */
			key_len = KNOWN_TDES_KEY_SIZE;
			key_alg = KNOWN_TDES_KEY_ALGORITHM;
		}		/* for i ... */

		printf("wrap: %d tests passed, %d tests failed\n",
		       passed_count, failed_count);

		*total_passed_count += passed_count;
		*total_failed_count += failed_count;

		/* Release the user keystore */
		fsl_shw_release_keystore(my_ctx, &keystore);
	}

	/* Clean up any allocated memory */
	if (encrypt_input) {
		free(encrypt_input);
	}
	if (decrypt_input) {
		free(decrypt_input);
	}
	if (encrypt_output) {
		free(encrypt_output);
	}
	if (decrypt_output) {
		free(decrypt_output);
	}
	if (blob) {
		free(blob);
	}

	return;
}

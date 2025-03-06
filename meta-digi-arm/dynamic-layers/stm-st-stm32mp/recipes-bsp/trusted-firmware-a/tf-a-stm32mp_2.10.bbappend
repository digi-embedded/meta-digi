#
# Copyright (C) 2022-2025, Digi International Inc.
#

# Select internal or Github TF-A repo
TFA_URI_STASH = "${DIGI_MTK_GIT}/emp/arm-trusted-firmware.git;protocol=ssh"
TFA_URI_GITHUB = "${DIGI_GITHUB_GIT}/arm-trusted-firmware.git;protocol=https"
TFA_GIT_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1' , '${TFA_URI_STASH}', '${TFA_URI_GITHUB}', d)}"

SRCBRANCH = "v2.10/stm32mp/maint"
SRCREV = "87e6c6d0d7990c5c5d25aaf53da50aa322ed232c"

SRC_URI = " \
    ${TFA_GIT_URI};nobranch=1 \
"

TF_A_CONFIG[nand]   = "${DEVICE_BOARD_ENABLE:NAND},STM32MP_RAW_NAND=1 ${@'STM32MP_FORCE_MTD_START_OFFSET=${TF_A_MTD_START_OFFSET_NAND}' if ${TF_A_MTD_START_OFFSET_NAND} else ''} STM32MP_USB_PROGRAMMER=1"
# TF_A_CONFIG[uart] (same as 'optee-programmer-uart')
TF_A_CONFIG[uart] ?= "\
    ${STM32MP_DEVICETREE_PROGRAMMER},\
    ${TF_A_CONFIG_OPTS_optee} STM32MP_UART_PROGRAMMER=1,\
    ${TF_A_CONFIG_BASENAME_BIN},\
    ${TF_A_CONFIG_MAKE_TARGET} ${TF_A_CONFIG_MAKE_EXTRAS},\
    ${TF_A_CONFIG_DEPLOY_FTYPE} ${TF_A_CONFIG_DEPLOY_EXTRA}"
# TF_A_CONFIG[usb] (same as 'optee-programmer-uart')
TF_A_CONFIG[usb] ?= "\
    ${STM32MP_DEVICETREE_PROGRAMMER},\
    ${TF_A_CONFIG_OPTS_optee} STM32MP_USB_PROGRAMMER=1,\
    ${TF_A_CONFIG_BASENAME_BIN},\
    ${TF_A_CONFIG_MAKE_TARGET} ${TF_A_CONFIG_MAKE_EXTRAS},\
    ${TF_A_CONFIG_DEPLOY_FTYPE} ${TF_A_CONFIG_DEPLOY_EXTRA}"

DEPENDS += " \
    ${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', 'trustfence-sign-tools-native', '', d)} \
"

# This dependency is required so that the PKI generation completes before
# proceeding with set_fip_sign_key() where we extract the password that
# is later used on the do_deploy of the fip-utils-stm32mp.bbclass.
do_install[depends] = " \
    trustfence-sign-tools-native:do_populate_sysroot \
    openssl-native:do_populate_sysroot \
"

# Generate PKI tree if it doesn't exist.
# This is an append to do_compile because in this recipe, the do_deploy
# task comes right after do_compile, and the keys must be ready before that.
do_compile:append() {
	if ${@oe.utils.conditional('TRUSTFENCE_SIGN','1','true','false',d)}; then
		check_gen_pki_tree
	fi
}

# Obtain password to use in FIP generation
# Get password from file using the given key index
do_deploy[prefuncs] += "${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', 'set_fip_sign_key', '', d)}"
python set_fip_sign_key() {
    passfile = d.getVar('TRUSTFENCE_PASSWORD_FILE')
    if (os.path.isfile(passfile)):
        with open(passfile, "r") as file:
            p = file.read().strip()
            if (p):
                d.setVar('FIP_SIGN_KEY_PASS', p)
}

# This runs after 'tf_a_sysroot_populate()' which populates all
# TF-A artifacts on the image deploy dir.
# The purpose of this function is to create symlinks to the files needed
# by the uuu installer that are located in subdirectories.
deploy_symlinks_atf() {
	# Remove trailing slash (/) from ST variable
	TF_A_BASEDIR="$(echo ${FIP_DIR_TFA_BASE} | cut -c2-)"
	unset i
	for config in ${TF_A_CONFIG}; do
		i=$(expr $i + 1)
		# Initialize devicetree list and tf-a basename
		dt_config=$(echo ${TF_A_DEVICETREE} | cut -d',' -f${i})
		tfa_basename=$(echo ${TF_A_BINARIES} | cut -d',' -f${i})
		for dt in ${dt_config}; do
			TF_A_FILENAME="${tfa_basename}-${dt}-${config}.${TF_A_SUFFIX}"
			if [ -f "${DEPLOY_DIR_IMAGE}/${TF_A_BASEDIR}/${TF_A_FILENAME}" ]; then
				cd "${DEPLOY_DIR_IMAGE}"
				# symlink TF-A
				ln -sf "${TF_A_BASEDIR}/${TF_A_FILENAME}" "${DEPLOY_DIR_IMAGE}/"
			fi
		done
	done

	# Last value of 'dt' is good for metadata binary, so use that.
	if [ "${TF_A_ENABLE_METADATA}" = "1" ]; then
		if [ -f "${DEPLOY_DIR_IMAGE}/${TF_A_BASEDIR}/${TF_A_METADATA_BINARY}" ]; then
			cd "${DEPLOY_DIR_IMAGE}"
			# symlink metadata
			ln -sf "${TF_A_BASEDIR}/${TF_A_METADATA_BINARY}" "${DEPLOY_DIR_IMAGE}/${TF_A_METADATA_NAME}-${dt}.${TF_A_METADATA_SUFFIX}"
		fi
	fi
}
SYSROOT_PREPROCESS_FUNCS += "deploy_symlinks_atf"

# Sign TF-A image
do_deploy[postfuncs] += "${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', 'tfa_sign', '', d)}"
tfa_sign() {
	export CONFIG_SIGN_KEYS_PATH="${TRUSTFENCE_SIGN_KEYS_PATH}"
	export CONFIG_KEY_INDEX="${TRUSTFENCE_KEY_INDEX}"

	unset i
	for config in ${TF_A_CONFIG}; do
		i=$(expr $i + 1)
		# Initialize devicetree list and tf-a basename
		dt_config=$(echo ${TF_A_DEVICETREE} | cut -d',' -f${i})
		tfa_basename=$(echo ${TF_A_BINARIES} | cut -d',' -f${i})
		tfa_file_type=$(echo ${TF_A_FILES} | cut -d',' -f${i})
		for dt in ${dt_config}; do
			for file_type in ${tfa_file_type}; do
				case "${file_type}" in
				bl2)
					TF_A_FILENAME="${tfa_basename}-${dt}-${config}.${TF_A_SUFFIX}"
					if [ -f "${DEPLOYDIR}/arm-trusted-firmware/${TF_A_FILENAME}" ]; then
						trustfence-sign-artifact.sh -p "${DIGI_SOM}" -t "${DEPLOYDIR}/arm-trusted-firmware/${TF_A_FILENAME}" "${DEPLOYDIR}/arm-trusted-firmware/${TF_A_FILENAME}${TFA_SIGN_SUFFIX}"
						# the generated artifact lacks 'w' permission which prevents deletion by the build system
						chmod u+w "${DEPLOYDIR}/arm-trusted-firmware/${TF_A_FILENAME}${TFA_SIGN_SUFFIX}"
						# symlink TF-A
						ln -s "arm-trusted-firmware/${TF_A_FILENAME}${TFA_SIGN_SUFFIX}" "${DEPLOYDIR}/"
					fi
				esac
			done # for file_type in ${tfa_file_type}
		done # for dt in ${dt_config}
	done # for config in ${TF_A_CONFIG}
}

#
# Copyright (C) 2022-2025, Digi International Inc.
#

# Select internal or Github TF-A repo
TFA_URI_STASH = "${DIGI_MTK_GIT}/emp/arm-trusted-firmware.git;protocol=ssh"
TFA_URI_GITHUB = "${DIGI_GITHUB_GIT}/arm-trusted-firmware.git;protocol=https"
TFA_GIT_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1' , '${TFA_URI_STASH}', '${TFA_URI_GITHUB}', d)}"

SRCBRANCH = "v2.10/stm32mp/master"
SRCREV = "${AUTOREV}"

SRC_URI = " \
    ${TFA_GIT_URI};branch=${SRCBRANCH} \
"

# stm32mp15 = header-version 1
SIGN_TOOL_EXTRA_soc:ccmp15 = " ${@bb.utils.contains('ENCRYPT_ENABLE', '1', '-of ${TF_A_ENCRYPT_OF}', '', d)}"
# stm32mp13 = header-version 2
SIGN_TOOL_EXTRA_soc:ccmp13 = " ${@bb.utils.contains('ENCRYPT_ENABLE', '1', '-of ${TF_A_ENCRYPT_OF}', '-of ${TF_A_SIGN_OF}', d)}"
# stm32mp2 = header-version 2.2
SIGN_TOOL_EXTRA_soc:stm32mp2common = " --header-version 2.2 ${@bb.utils.contains('ENCRYPT_ENABLE', '1', '-of ${TF_A_ENCRYPT_OF}', '-of ${TF_A_SIGN_OF}', d)}"

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
# This is an prepend to do_compile because in this recipe, the keys
# must be ready before that.
do_generate_pki_tree() {
	if ${@oe.utils.conditional('TRUSTFENCE_SIGN','1','true','false',d)}; then
		check_gen_pki_tree
	fi
}
addtask generate_pki_tree before do_compile after do_configure

# Obtain password to use in TF-A generation
# Get password from file using the given key index
do_compile[prefuncs] += "${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', 'set_tfa_sign_key', '', d)}"
python set_tfa_sign_key() {
    passfile = d.getVar('TRUSTFENCE_PASSWORD_FILE')
    if (os.path.isfile(passfile)):
        with open(passfile, "r") as file:
            p = file.read().strip()
            if (p):
                d.setVar('SIGN_KEY_PASS', p)
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

#
# Copyright (C) 2022 Digi International Inc.
#

# Select internal or Github TF-A repo
TFA_URI_STASH = "${DIGI_MTK_GIT}/emp/arm-trusted-firmware.git;protocol=ssh"
TFA_URI_GITHUB = "${DIGI_GITHUB_GIT}/arm-trusted-firmware.git;protocol=https"
TFA_GIT_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1' , '${TFA_URI_STASH}', '${TFA_URI_GITHUB}', d)}"

SRCBRANCH = "v2.6/stm32mp/master"
SRCREV = "${AUTOREV}"

SRC_URI = " \
    ${TFA_GIT_URI};branch=${SRCBRANCH} \
"

TF_A_CONFIG[nand]   = "${DEVICE_BOARD_ENABLE:NAND},STM32MP_RAW_NAND=1 ${@'STM32MP_FORCE_MTD_START_OFFSET=${TF_A_MTD_START_OFFSET_NAND}' if ${TF_A_MTD_START_OFFSET_NAND} else ''} STM32MP_USB_PROGRAMMER=1"

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
						trustfence-sign-artifact.sh -p "${DIGI_SOM}" -t "${DEPLOYDIR}/arm-trusted-firmware/${TF_A_FILENAME}" "${DEPLOYDIR}/arm-trusted-firmware/${TF_A_FILENAME}_signed"
					fi
				esac
			done # for file_type in ${tfa_file_type}
		done # for dt in ${dt_config}
	done # for config in ${TF_A_CONFIG}
}

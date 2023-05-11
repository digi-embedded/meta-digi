# Copyright (C) 2023, Digi International Inc.

SUMMARY = "TrustFence generation of Public Key Infrastructure (PKI)"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

inherit native

RDEPENDS:${PN} = " \
    trustfence-sign-tools-native \
    openssl-native \
"

S = "${WORKDIR}"

do_fetch[noexec] = "1"
do_configure[noexec] = "1"
do_compile[noexec] = "1"

# Function to generate a PKI tree (with lock dir protection)
GENPKI_LOCK_DIR = "${TRUSTFENCE_SIGN_KEYS_PATH}/.genpki.lock"
gen_pki_tree() {
	if mkdir -p ${GENPKI_LOCK_DIR}; then
		if [ "${DEY_SOC_VENDOR}" = "NXP" ]; then
			trustfence-gen-pki.sh ${TRUSTFENCE_SIGN_KEYS_PATH}
		elif [ "${DEY_SOC_VENDOR}" = "STM" ]; then
			# Call sign script with no artifact arguments to just
			# generate the keys
			export CONFIG_SIGN_KEYS_PATH="${TRUSTFENCE_SIGN_KEYS_PATH}"
			export CONFIG_KEY_INDEX="${TRUSTFENCE_KEY_INDEX}"
			trustfence-sign-artifact.sh -p ${DIGI_SOM}
		fi
		rm -rf ${GENPKI_LOCK_DIR}
	else
		bbfatal "Could not get lock to generate PKI tree"
	fi
}

# Function that generates a PKI tree if there isn't one
check_gen_pki_tree() {
	if [ "${DEY_SOC_VENDOR}" = "NXP" ]; then
		SRK_KEYS="$(echo ${TRUSTFENCE_SIGN_KEYS_PATH}/crts/SRK*crt.pem | sed s/\ /\,/g)"
		n_commas="$(echo ${SRK_KEYS} | grep -o "," | wc -l)"
		if [ "${n_commas}" -eq 0 ]; then
			gen_pki_tree
		elif [ "${n_commas}" -ne 3 ]; then
			bbfatal "Inconsistent PKI tree"
		fi
	elif [ "${DEY_SOC_VENDOR}" = "STM" ]; then
		# The script that generates the PKI tree already checks if
		# there isn't one, so there's nothing to do here but calling it.
		gen_pki_tree
	fi
}

do_install[depends] = "trustfence-sign-tools-native:do_populate_sysroot \
			openssl-native:do_populate_sysroot"
do_install() {
	check_gen_pki_tree
}

FILES:${PN} = "${bindir}"

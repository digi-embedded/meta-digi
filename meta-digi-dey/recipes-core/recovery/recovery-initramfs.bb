# Copyright (C) 2016, 2017 Digi International Inc.

SUMMARY = "Recovery initramfs files"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

DEPENDS += "${@base_conditional('TRUSTFENCE_SIGN', '1', 'trustfence-cst-native openssl-native', '', d)}"

SRC_URI = " \
    file://recovery-initramfs-init \
    file://swupdate.cfg \
"

S = "${WORKDIR}"

do_install() {
	install -d ${D}${sysconfdir}
	install -m 0755 ${WORKDIR}/recovery-initramfs-init ${D}/init
	install -m 0644 ${WORKDIR}/swupdate.cfg ${D}${sysconfdir}

	# If Trustfence is enabled, copy the public key that is going to be used into the
	# initramfs '/etc/ssl/certs' folder in order to verify swupdate packages.
	if [ "${TRUSTFENCE_SIGN}" = "1" ]; then
		# Retrieve the key index to use.
		KEY_INDEX="0"
		if [ -n "${TRUSTFENCE_KEY_INDEX}" ]; then
			KEY_INDEX="${TRUSTFENCE_KEY_INDEX}"
		fi
		KEY_INDEX_1=$(expr ${KEY_INDEX} + 1)

		# Check if keys are already generated or not. If keys do not exist, generate them.
		SRK_KEYS="$(echo ${TRUSTFENCE_SIGN_KEYS_PATH}/crts/SRK*crt.pem | sed s/\ /\,/g)"
		CERT_CSF="$(echo ${TRUSTFENCE_SIGN_KEYS_PATH}/crts/CSF${KEY_INDEX_1}*crt.pem)"
		CERT_IMG="$(echo ${TRUSTFENCE_SIGN_KEYS_PATH}/crts/IMG${KEY_INDEX_1}*crt.pem)"
		n_commas="$(echo ${SRK_KEYS} | grep -o "," | wc -l)"
		if [ "${n_commas}" -eq 3 ] && [ -f "${CERT_CSF}" ] && [ -f "${CERT_IMG}" ]; then
			# PKI tree already exists. Do nothing
			echo "Using existing PKI tree for recovery."
		elif [ "${n_commas}" -eq 0 ] || [ ! -f "${CERT_CSF}" ] || [ ! -f "${CERT_IMG}" ]; then
			# Generate PKI
			mkdir -p "${TRUSTFENCE_SIGN_KEYS_PATH}"
			trustfence-gen-pki.sh "${TRUSTFENCE_SIGN_KEYS_PATH}"
			CERT_IMG="$(echo ${TRUSTFENCE_SIGN_KEYS_PATH}/crts/IMG${KEY_INDEX_1}*crt.pem)"
		else
			echo "Inconsistent CST folder."
			exit 1
		fi

		# Extract the public key.
		install -d ${D}${sysconfdir}/ssl/certs
		openssl x509 -pubkey -noout -in "${CERT_IMG}" > ${D}${sysconfdir}/ssl/certs/key.pub
	fi
}

# Do not create debug/devel packages
PACKAGES = "${PN}"

FILES_${PN} = "/"

RDEPENDS_${PN}_append_ccimx6 = " \
    cryptsetup \
    rng-tools \
"

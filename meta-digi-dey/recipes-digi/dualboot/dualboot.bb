# Copyright (C) 2021-2023 Digi International Inc.

SUMMARY = "Digi Embedded Yocto Dual boot support"
SECTION = "base"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

SOC_SIGN_DEPENDS = " \
    ${@oe.utils.conditional('DEY_SOC_VENDOR', 'NXP', 'trustfence-cst-native', '', d)} \
"
DEPENDS += "${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', \
		'openssl-native ' \
		'trustfence-sign-tools-native ' \
		'${SOC_SIGN_DEPENDS}', '', d)}"

SRC_URI = " \
    file://dualboot-init \
    file://update-firmware \
    file://firmware-update-check.service \
"

S = "${WORKDIR}"

inherit systemd update-rc.d

do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install() {
	install -d ${D}${sysconfdir}/init.d/
	install -m 0755 ${WORKDIR}/dualboot-init ${D}${sysconfdir}/dualboot-init
	ln -sf /etc/dualboot-init ${D}${sysconfdir}/init.d/dualboot-init

	install -d ${D}${bindir}
	install -m 0755 ${WORKDIR}/update-firmware ${D}${bindir}

	install -d ${D}${systemd_unitdir}/system/
	install -m 0644 ${WORKDIR}/firmware-update-check.service ${D}${systemd_unitdir}/system/

	# If Trustfence is enabled, copy the public key that is going to be used into the
	# initramfs '/etc/ssl/certs' folder in order to verify swupdate packages.
	if [ "${TRUSTFENCE_SIGN}" = "1" ]; then
		# Retrieve the key index to use.
		KEY_INDEX="0"
		if [ -n "${TRUSTFENCE_KEY_INDEX}" ]; then
			KEY_INDEX="${TRUSTFENCE_KEY_INDEX}"
		fi
		KEY_INDEX_1=$(expr ${KEY_INDEX} + 1)

		# Find the certificate to use.
		if [ "${DEY_SOC_VENDOR}" = "NXP" ]; then
			if [ "${TRUSTFENCE_SIGN_MODE}" = "HAB" ]; then
				CERT_IMG="$(echo ${TRUSTFENCE_SIGN_KEYS_PATH}/crts/IMG${KEY_INDEX_1}*crt.pem)"
			elif [ "${TRUSTFENCE_SIGN_MODE}" = "AHAB" ]; then
				CERT_IMG="$(echo ${TRUSTFENCE_SIGN_KEYS_PATH}/crts/SRK${KEY_INDEX_1}*_ca_crt.pem)"
			else
				bberror "Unknown TRUSTFENCE_SIGN_MODE value"
				exit 1
			fi
			# Extract the public key from the certificate.
			install -d ${D}${sysconfdir}/ssl/certs
			openssl x509 -pubkey -noout -in "${CERT_IMG}" > ${D}${sysconfdir}/ssl/certs/key.pub
		elif [ "${DEY_SOC_VENDOR}" = "STM" ]; then
			# Copy the public key to the rootfs
			if [ "${DIGI_SOM}" = "ccmp15" ]; then
				PUBLIC_KEY="${TRUSTFENCE_SIGN_KEYS_PATH}/keys/publicKey00.pem"
			elif [ "${DIGI_SOM}" = "ccmp13" ]; then
				PUBLIC_KEY="${TRUSTFENCE_SIGN_KEYS_PATH}/keys/publicKey0${KEY_INDEX}.pem"
			else
				bberror "Unknown DIGI_SOM"
				exit 1
			fi
			install -d ${D}${sysconfdir}/ssl/certs
			cp ${PUBLIC_KEY} ${D}${sysconfdir}/ssl/certs/key.pub
		fi
	fi
}

FILES:${PN} += " \
    ${sysconfdir}/dualboot-init \
    ${sysconfdir}/init.d/dualboot-init \
    ${bindir}/update-firmware \
    ${systemd_unitdir}/system/firmware-update-check.service \
    ${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', '${sysconfdir}/ssl/certs/key.pub', '', d)} \
"

INITSCRIPT_NAME = "dualboot-init"
INITSCRIPT_PARAMS = "start 19 2 3 4 5 . stop 21 0 1 6 ."

SYSTEMD_SERVICE:${PN} = "firmware-update-check.service"

RDEPENDS:${PN} += "swupdate"

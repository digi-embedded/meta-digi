# Copyright (C) 2016, 2017 Digi International Inc.

SUMMARY = "Recovery initramfs files"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

# When building a TrustFence enabled recovery initramfs, we need the TrustFence PKI tree to
# be already generated in order to copy the public key. Forcing a dependence against 
# 'virtual/kernel' ensures that the keys are already generated as they are needed to sign the
# kernel artifacts.
DEPENDS += "${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', 'virtual/kernel openssl-native', '', d)}"

SRC_URI = " \
    file://recovery-initramfs-init \
    file://swupdate.cfg \
    file://automount_block.sh \
    file://automount_mtd.sh \
    file://mdev.conf \
"

S = "${WORKDIR}"

do_install() {
	install -d ${D}${sysconfdir}
	install -m 0755 ${WORKDIR}/recovery-initramfs-init ${D}/init
	install -m 0644 ${WORKDIR}/swupdate.cfg ${D}${sysconfdir}
	install -d ${D}${base_libdir}/mdev
	install -m 0755 ${WORKDIR}/automount_block.sh ${D}${base_libdir}/mdev/automount_block.sh
	install -m 0755 ${WORKDIR}/automount_mtd.sh ${D}${base_libdir}/mdev/automount_mtd.sh
	install -m 0644 ${WORKDIR}/mdev.conf ${D}${sysconfdir}

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
		CERT_IMG="$(echo ${TRUSTFENCE_SIGN_KEYS_PATH}/crts/IMG${KEY_INDEX_1}*crt.pem)"

		# Extract the public key from the certificate.
		install -d ${D}${sysconfdir}/ssl/certs
		openssl x509 -pubkey -noout -in "${CERT_IMG}" > ${D}${sysconfdir}/ssl/certs/key.pub
	fi
}

# Do not create debug/devel packages
PACKAGES = "${PN}"

FILES_${PN} = "/"

RDEPENDS_${PN}_append = " \
    cryptsetup \
"

# Copyright (C) 2017-2022, Digi International Inc.

SUMMARY = "Digi's device cloud connector"
SECTION = "libs"
LICENSE = "MPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MPL-2.0;md5=815ca599c9df247a0c7f619bab123dad"

DEPENDS = "libconfuse libdigiapix openssl recovery-utils swupdate zlib json-c"

SRCBRANCH = "master"
SRCREV = "${AUTOREV}"

CC_STASH = "gitsm://git@stash.digi.com/cc/cc_dey.git;protocol=ssh"
CC_GITHUB = "gitsm://github.com/digi-embedded/cc_dey.git;protocol=https"

CC_GIT_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1' , '${CC_STASH}', '${CC_GITHUB}', d)}"

CC_DEVICE_TYPE ?= "${MACHINE}"

SRC_URI = " \
    ${CC_GIT_URI};branch=${SRCBRANCH} \
    file://cloud-connector-init \
    file://cloud-connector.service \
"

S = "${WORKDIR}/git"

inherit pkgconfig systemd update-rc.d

do_install() {
	oe_runmake DESTDIR=${D} install

	if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
		# Install systemd unit files
		install -d ${D}${systemd_unitdir}/system
		install -m 0644 ${WORKDIR}/cloud-connector.service ${D}${systemd_unitdir}/system/
	fi

	install -d ${D}${sysconfdir}/init.d/
	install -m 755 ${WORKDIR}/cloud-connector-init ${D}${sysconfdir}/cloud-connector
	ln -sf /etc/cloud-connector ${D}${sysconfdir}/init.d/cloud-connector

	# Set the device type. Its maximum length is 255 characters
	[ -z "${CC_DEVICE_TYPE}" ] && device_type="${MACHINE}" || device_type="${CC_DEVICE_TYPE}"
	device_type="$(echo "${device_type}" | cut -c1-255)"
	sed -i "/device_type = .*/c\device_type = \"${device_type}\"" ${D}${sysconfdir}/cc.conf
}

do_install:append:ccimx6ul() {
	sed -i "/url = \"edp12.devicecloud.com\"/c\url = \"remotemanager.digi.com\"" ${D}${sysconfdir}/cc.conf
	sed -i "/client_cert_path = \"\/mnt\/data\/drm_cert.pem\"/c\client_cert_path = \"\/etc\/ssl\/certs\/drm_cert.pem\"" ${D}${sysconfdir}/cc.conf
}

pkg_postinst_ontarget:${PN}() {
	# If dualboot is enabled, change the CloudConnector download path on the first boot
	if [ "$(fw_printenv -n dualboot 2>/dev/null)" = "yes" ]; then
		sed -i "/firmware_download_path = \/mnt\/update/c\firmware_download_path = \/home\/root" /etc/cc.conf
	fi
}

inherit ${@bb.utils.contains("IMAGE_FEATURES", "read-only-rootfs", "remove-pkg-postinst-ontarget", "", d)}

INITSCRIPT_NAME = "cloud-connector"
SYSTEMD_SERVICE:${PN} = "cloud-connector.service"

PACKAGES =+ "${PN}-cert"

FILES:${PN} += " \
    ${systemd_unitdir}/system/cloud-connector.service \
    ${sysconfdir}/cloud-connector \
    ${sysconfdir}/init.d/cloud-connector \
"

FILES:${PN}-cert = "${sysconfdir}/ssl/certs/Digi_Int-ca-cert-public.crt"

CONFFILES:${PN} += "${sysconfdir}/cc.conf"

RDEPENDS:${PN} = "${PN}-cert"

# 'cloud-connector-init' script uses '/etc/init.d/functions'
RDEPENDS:${PN} += "initscripts-functions"

# Disable extra compilation checks from SECURITY_CFLAGS to avoid build errors
lcl_maybe_fortify:pn-cloudconnector = ""

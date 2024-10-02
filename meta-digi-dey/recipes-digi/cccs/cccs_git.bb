# Copyright (C) 2017-2024, Digi International Inc.

SUMMARY = "Digi's ConnectCore Cloud services"
SECTION = "libs"
LICENSE = "MPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MPL-2.0;md5=815ca599c9df247a0c7f619bab123dad"

DEPENDS = "libconfuse libdigiapix openssl recovery-utils swupdate zlib json-c"

SRCBRANCH = "dey-4.0/maint"
SRCREV = "${AUTOREV}"

CC_STASH = "gitsm://git@stash.digi.com/cc/cc_dey.git;protocol=ssh"
CC_GITHUB = "gitsm://github.com/digi-embedded/cc_dey.git;protocol=https"

CC_GIT_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1' , '${CC_STASH}', '${CC_GITHUB}', d)}"

CCCS_DEVICE_TYPE ?= "${MACHINE}"
CCCS_CONF_PATH ?= ""

SRC_URI = " \
    ${CC_GIT_URI};branch=${SRCBRANCH} \
    file://cccsd-init \
    file://cccsd.service \
    file://cccs-gs-demo-init \
    file://cccs-gs-demo.service \
"
SRC_URI:append = "${@oe.utils.ifelse(d.getVar('CCCS_CONF_PATH'), \
                     oe.utils.ifelse(d.getVar('CCCS_CONF_PATH').startswith('/'), "file://%s" % d.getVar('CCCS_CONF_PATH'), d.getVar('CCCS_CONF_PATH')), '')}"

S = "${WORKDIR}/git"

# The configuration file can be provided by the user, so provide a PREMIRROR to
# a local directory that can be configured in the project's local.conf file
# using CCCS_CONF_PATH variable.
python() {
    cccs_conf_path = d.getVar('CCCS_CONF_PATH')
    if cccs_conf_path:
        premirrors = d.getVar('PREMIRRORS')
        if cccs_conf_path.startswith('/'):
            cccs_conf_path = "file://%s" % cccs_conf_path
        d.setVar('PREMIRRORS', "%s %s \\n %s" % (cccs_conf_path, cccs_conf_path, premirrors))
        cccs_conf_sha256 = d.getVar('CCCS_CONF_SHA256')
        if cccs_conf_sha256:
            d.setVarFlag("SRC_URI", "sha256sum", cccs_conf_sha256)
}

inherit pkgconfig systemd update-rc.d

do_install() {
	oe_runmake DESTDIR=${D} install

	if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
		# Install systemd unit files
		install -d ${D}${systemd_unitdir}/system
		install -m 0644 ${WORKDIR}/cccsd.service ${D}${systemd_unitdir}/system/
		install -m 0644 ${WORKDIR}/cccs-gs-demo.service ${D}${systemd_unitdir}/system/
	fi

	install -d ${D}${sysconfdir}/init.d/
	install -m 755 ${WORKDIR}/cccsd-init ${D}${sysconfdir}/cccsd
	ln -sf /etc/cccsd ${D}${sysconfdir}/init.d/cccsd
	install -m 755 ${WORKDIR}/cccs-gs-demo-init ${D}${sysconfdir}/cccs-gs-demo
	ln -sf /etc/cccs-gs-demo ${D}${sysconfdir}/init.d/cccs-gs-demo

	if [ -n "${CCCS_CONF_PATH}" ]; then
		CONF="${CCCS_CONF_PATH}"
		if [ "${CONF#file://}" != "${CONF}" ]; then
			CONF="${CONF#file://}"
		elif [ "${CONF#/}" != "${CONF}" ]; then
			CONF="${CONF}"
		else
			CONF="${WORKDIR}/$(basename ${CONF})"
		fi
		install -m 0644 "${CONF}" ${D}${sysconfdir}/cccs.conf
	else
		# Set the device type. Its maximum length is 255 characters
		[ -z "${CCCS_DEVICE_TYPE}" ] && device_type="${MACHINE}" || device_type="${CCCS_DEVICE_TYPE}"
		device_type="$(echo "${device_type}" | cut -c1-255)"
		sed -i "/device_type = .*/c\device_type = \"${device_type}\"" ${D}${sysconfdir}/cccs.conf
	fi
}

do_install:append:ccimx6ul() {
	if [ -z "${CCCS_CONF_PATH}" ]; then
		sed -i "/url = \"edp12.devicecloud.com\"/c\url = \"remotemanager.digi.com\"" ${D}${sysconfdir}/cccs.conf
		sed -i "/client_cert_path = \"\/mnt\/data\/drm_cert.pem\"/c\client_cert_path = \"\/etc\/ssl\/certs\/drm_cert.pem\"" ${D}${sysconfdir}/cccs.conf
	fi
}

pkg_postinst_ontarget:${PN}-daemon() {
	# If dualboot is enabled, change the CCCSD download path and set on the fly to yes on the first boot
	if [ "$(fw_printenv -n dualboot 2>/dev/null)" = "yes" ]; then
		sed -i "/firmware_download_path = \/mnt\/update/c\firmware_download_path = \/home\/root" /etc/cccs.conf
		sed -i "/on_the_fly = false/c\on_the_fly = true" /etc/cccs.conf
	fi
}

inherit ${@bb.utils.contains("IMAGE_FEATURES", "read-only-rootfs", "remove-pkg-postinst-ontarget", \
           oe.utils.ifelse(d.getVar("CCCS_CONF_PATH"), "remove-pkg-postinst-ontarget", ""), d)}

INITSCRIPT_PACKAGES = "${PN}-daemon ${PN}-gs-demo"
INITSCRIPT_NAME:${PN}-daemon = "cccsd"
INITSCRIPT_PARAMS:${PN}-daemon = "defaults 19 81"
INITSCRIPT_NAME:${PN}-gs-demo = "cccs-gs-demo"
INITSCRIPT_PARAMS:${PN}-gs-demo = "defaults 81 19"

SYSTEMD_PACKAGES = "${PN}-daemon ${PN}-gs-demo"
SYSTEMD_SERVICE:${PN}-daemon = "cccsd.service"
SYSTEMD_SERVICE:${PN}-gs-demo = "cccs-gs-demo.service"

PACKAGES =+ " \
    ${PN}-cert \
    ${PN}-daemon \
    ${PN}-gs-demo \
    ${PN}-legacy \
    ${PN}-legacy-dev \
    ${PN}-legacy-staticdev \
"

FILES:${PN}-cert = "${sysconfdir}/ssl/certs/Digi_Int-ca-cert-public.crt"

FILES:${PN}-daemon = " \
    ${bindir}/cccsd \
    ${systemd_unitdir}/system/cccsd.service \
    ${sysconfdir}/cccsd \
    ${sysconfdir}/cccs.conf \
    ${sysconfdir}/init.d/cccsd \
"

FILES:${PN}-gs-demo = " \
    ${bindir}/cccs-gs-demo \
    ${systemd_unitdir}/system/cccs-gs-demo.service \
    ${sysconfdir}/cccs-gs-demo \
"

FILES:${PN}-legacy = " \
    ${bindir}/cloud-connector \
    ${sysconfdir}/cc.conf \
"

FILES:${PN}-legacy-dev = " \
    ${includedir}/cloudconnector \
    ${libdir}/pkgconfig/cloudconnector.pc \
"

FILES:${PN}-legacy-staticdev = " \
    ${libdir}/libcloudconnector.a \
"

CONFFILES:${PN}-daemon += "${sysconfdir}/cccs.conf"

CONFFILES:${PN}-legacy += "${sysconfdir}/cc.conf"

# 'cccsd-init' script uses '/etc/init.d/functions'
RDEPENDS:${PN}-daemon = " \
    ${PN} \
    ${PN}-cert \
    initscripts-functions \
"

# 'cccsd-gs-demo-init' script uses '/etc/init.d/functions'
RDEPENDS:${PN}-gs-demo = " \
    ${PN}-daemon \
    initscripts-functions \
"

RDEPENDS:${PN}-legacy = "${PN} ${PN}-cert"

# Disable extra compilation checks from SECURITY_CFLAGS to avoid build errors
lcl_maybe_fortify:pn-cccs = ""

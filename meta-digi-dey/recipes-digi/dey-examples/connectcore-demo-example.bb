# Copyright (C) 2022 Digi International.

require dey-examples-src.inc

SUMMARY = "Connectcore demo"
DESCRIPTION = "Connectcore demo"
SECTION = "examples"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

SRCBRANCH = "dey-3.2/maint"
SRCREV = "${AUTOREV}"

DEY_EXAMPLES_STASH = "${DIGI_MTK_GIT}/dey/dey-examples.git;protocol=ssh"
DEY_EXAMPLES_GITHUB = "${DIGI_GITHUB_GIT}/dey-examples.git;protocol=https"
DEY_EXAMPLES_GIT_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1' , '${DEY_EXAMPLES_STASH}', '${DEY_EXAMPLES_GITHUB}', d)}"

SRC_URI = " \
    ${DEY_EXAMPLES_GIT_URI};branch=${SRCBRANCH} \
    file://connectcore-demo-example/connectcore-demo-example-init \
    file://connectcore-demo-example/connectcore-demo-example.service \
"

RDEPENDS_${PN} = " \
    cog \
    python3-core \
    video-examples \
    webglsamples \
"

S = "${WORKDIR}/connectcore-demo-example"

inherit systemd update-rc.d

do_install() {
    install -d ${D}/srv/www
    cp -r ${WORKDIR}/git/connectcore-demo-example/* ${D}/srv/www/

    if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
        # Install systemd unit files
        install -d ${D}${systemd_unitdir}/system
        install -m 0644 ${S}/connectcore-demo-example.service ${D}${systemd_unitdir}/system/
    fi

    # connectcore-demo-example-init
    install -d ${D}${sysconfdir}/init.d/
    install -m 755 ${S}/connectcore-demo-example-init ${D}${sysconfdir}/connectcore-demo-example
    ln -sf /etc/connectcore-demo-example ${D}${sysconfdir}/init.d/connectcore-demo-example
}

FILES_${PN} += " \
    /srv/www/* \
    ${systemd_unitdir}/system/connectcore-demo-example.service \
    ${sysconfdir}/connectcore-demo-example \
    ${sysconfdir}/init.d/connectcore-demo-example \
"

INITSCRIPT_NAME = "connectcore-demo-example"
INITSCRIPT_PARAMS = "start 19 2 3 4 5 . stop 21 0 1 6 ."

SYSTEMD_SERVICE_${PN} = "connectcore-demo-example.service"

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(ccimx8x|ccimx8m)"

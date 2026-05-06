# Copyright (C) 2026, Digi International Inc.

SUMMARY = "Digi ConnectCore Container Manager daemon and CLI"
DESCRIPTION = "Local daemon and CLI to manage Podman/LXC container lifecycle on ConnectCore devices."
HOMEPAGE = "https://github.com/digi-embedded/cc-container-mng"
SECTION = "base"

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=266029e5b51b2fa808364134ee8ec673"

SRCBRANCH ?= "master"
SRCREV = "${AUTOREV}"
PV = "0.1.0+git${SRCPV}"

CC_CONTAINER_MNG_URI_STASH = "${DIGI_MTK_GIT}/dey/cc-container-mng.git;protocol=ssh"
CC_CONTAINER_MNG_URI_GITHUB = "${DIGI_GITHUB_GIT}/cc-container-mng.git;protocol=https"
CC_CONTAINER_MNG_GIT_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1', '${CC_CONTAINER_MNG_URI_STASH}', '${CC_CONTAINER_MNG_URI_GITHUB}', d)}"

SRC_URI = " \
    ${CC_CONTAINER_MNG_GIT_URI};branch=${SRCBRANCH} \
    file://cc-containerd.service \
    file://cc-containerd-shutdown.service \
"

S = "${WORKDIR}/git"

inherit python_setuptools_build_meta systemd

SYSTEMD_SERVICE:${PN} = "cc-containerd.service cc-containerd-shutdown.service"
SYSTEMD_AUTO_ENABLE ?= "enable"

RDEPENDS:${PN} += " \
    python3-core \
    python3-asyncio \
    python3-json \
    python3-logging \
"

do_install:append() {
    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${WORKDIR}/cc-containerd.service ${D}${systemd_system_unitdir}/cc-containerd.service
    install -m 0644 ${WORKDIR}/cc-containerd-shutdown.service ${D}${systemd_system_unitdir}/cc-containerd-shutdown.service

    install -d ${D}${sysconfdir}
    install -m 0644 ${S}/cc-container-mng.conf ${D}${sysconfdir}/cc-container-mng.conf

    sed -i \
        -e "s|\"/opt/cc-container|\"${CC_CONTAINER_PATH}|g" \
        ${D}${sysconfdir}/cc-container-mng.conf
}

FILES:${PN}:append = " \
    ${systemd_system_unitdir}/cc-containerd.service \
    ${systemd_system_unitdir}/cc-containerd-shutdown.service \
    ${sysconfdir}/cc-container-mng.conf \
"

CONFFILES:${PN} += "${sysconfdir}/cc-container-mng.conf"

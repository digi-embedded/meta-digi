# Copyright (C) 2026, Digi International Inc.

SUMMARY = "Digi ConnectCore Container Manager daemon and CLI"
DESCRIPTION = "Local daemon and CLI to manage Podman/LXC container lifecycle on ConnectCore devices."
SECTION = "base"

LICENSE = "CLOSED"

CC_CONTAINER_WHEEL = "digi_cc_container-${PV}-py3-none-any.whl"

SRC_URI = " \
    ${DIGI_PKG_SRC}/${BP}.tar.gz;name=archive \
    file://cc-containerd \
    file://containerctl \
    file://cc-containerd.service \
    file://cc-containerd-shutdown.service \
"
SRC_URI[archive.md5sum] = "6d84c6f5ec9dc94d542c91001ff5fd36"
SRC_URI[archive.sha256sum] = "ce24c4fde041a69a7646eb9bad4891d2eb91291f3534e71444552d3830247aaa"

S = "${WORKDIR}/${BP}"

inherit python3native python3-dir systemd

SYSTEMD_SERVICE:${PN} = "cc-containerd.service cc-containerd-shutdown.service"
SYSTEMD_AUTO_ENABLE ?= "enable"

DEPENDS += "python3-pip-native"

CONTAINERS_BACKEND_TOOLS ?= "podman lxc"

RDEPENDS:${PN} += " \
    ${CONTAINERS_BACKEND_TOOLS} \
    python3-core \
    python3-asyncio \
    python3-json \
    python3-logging \
"

do_install() {
    nativepython3 -m pip install \
        --no-deps \
        --no-index \
        --no-compile \
        --root=${D} \
        --prefix=${prefix} \
        ${S}/${CC_CONTAINER_WHEEL}

    rm -f ${D}${PYTHON_SITEPACKAGES_DIR}/digi_cc_container-${PV}.dist-info/direct_url.json

    install -d ${D}${bindir}
    install -m 0755 ${WORKDIR}/cc-containerd ${D}${bindir}/cc-containerd
    install -m 0755 ${WORKDIR}/containerctl ${D}${bindir}/containerctl

    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${WORKDIR}/cc-containerd.service \
        ${D}${systemd_system_unitdir}/cc-containerd.service
    install -m 0644 ${WORKDIR}/cc-containerd-shutdown.service \
        ${D}${systemd_system_unitdir}/cc-containerd-shutdown.service

    install -d ${D}${sysconfdir}
    install -m 0644 ${S}/cc-container-mng.conf \
        ${D}${sysconfdir}/cc-container-mng.conf

    sed -i \
        's|"working_path".*|"working_path": "${CC_CONTAINER_PATH}",|' \
        ${D}${sysconfdir}/cc-container-mng.conf
}

FILES:${PN}:append = " \
    ${PYTHON_SITEPACKAGES_DIR}/digi \
    ${PYTHON_SITEPACKAGES_DIR}/digi_cc_container-*.dist-info \
    ${systemd_system_unitdir}/cc-containerd.service \
    ${systemd_system_unitdir}/cc-containerd-shutdown.service \
"

CONFFILES:${PN} += "${sysconfdir}/cc-container-mng.conf"

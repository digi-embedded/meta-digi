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
SRC_URI[archive.md5sum] = "2ae2c3c09e9bf223e7de4ec0994376f3"
SRC_URI[archive.sha256sum] = "627d90eb53a48bf978fb6993f661af3dba8e6bf091d295ff481457e4c0cb96a0"

S = "${WORKDIR}/${BP}"

inherit python3native python3-dir systemd

SYSTEMD_SERVICE:${PN} = "cc-containerd.service cc-containerd-shutdown.service"
SYSTEMD_AUTO_ENABLE ?= "enable"

DEPENDS += "python3-pip-native"

RDEPENDS:${PN} += " \
    lxc \
    podman \
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

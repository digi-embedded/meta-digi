# Copyright (C) 2024, 2025, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI:append = " \
    file://sd_w61x_v1.bin.se \
    file://sduart_nw61x_v1.bin.se \
    file://uartspi_n61x_v1.bin.se \
"

do_install:append() {
    install -d ${D}${nonarch_base_libdir}/firmware/nxp

    install -m 0644 ${WORKDIR}/sd_w61x_v1.bin.se ${D}${nonarch_base_libdir}/firmware/nxp/
    install -m 0644 ${WORKDIR}/sduart_nw61x_v1.bin.se ${D}${nonarch_base_libdir}/firmware/nxp/
    install -m 0644 ${WORKDIR}/uartspi_n61x_v1.bin.se ${D}${nonarch_base_libdir}/firmware/nxp/
}

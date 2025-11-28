# Copyright (C) 2024, 2025, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI += "${@oe.utils.vartrue('TRUSTFENCE_FILE_BASED_ENCRYPT', 'file://tee-supplicant', '', d)}"

do_install:append(){
    if ${@oe.utils.vartrue('TRUSTFENCE_FILE_BASED_ENCRYPT', 'true', 'false',d)}; then
        install -d ${D}${sysconfdir}/default/
        install -m 0644 ${WORKDIR}/tee-supplicant ${D}${sysconfdir}/default/tee-supplicant
    fi
}

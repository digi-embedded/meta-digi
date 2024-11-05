# Copyright 2024 Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI:append:ccimx93 = " file://ethosu_firmware"

UNPACK_POSTFUNC = ""
UNPACK_POSTFUNC:ccimx93 = "copy_ethos_u_fw"
copy_ethos_u_fw() {
	cp -f ${WORKDIR}/ethosu_firmware ${S}
}
do_unpack[postfuncs] += "${UNPACK_POSTFUNC}"

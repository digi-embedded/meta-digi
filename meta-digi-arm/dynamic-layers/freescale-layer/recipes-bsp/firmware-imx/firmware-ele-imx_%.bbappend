# Copyright 2024,2025 Digi International Inc.
FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

# Forward-port the i.MX93 A0 fw from v0.1.0
SRC_URI:append:ccimx93 = " file://mx93a0-ahab-container.img"

UNPACK_POSTFUNC = ""
UNPACK_POSTFUNC:ccimx93 = "copy_ele_a0_fw"
copy_ele_a0_fw() {
	cp -f ${WORKDIR}/mx93a0-ahab-container.img ${S}
}
do_unpack[postfuncs] += "${UNPACK_POSTFUNC}"

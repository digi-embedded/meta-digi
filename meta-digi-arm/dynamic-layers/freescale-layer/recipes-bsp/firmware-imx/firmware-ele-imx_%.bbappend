# Copyright 2024 Digi International Inc.
FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

# Forward-port the i.MX93 A0 fw from v0.1.0
SRC_URI:append:ccimx93 = " file://mx93a0-ahab-container.img"

IMX_SRCREV_ABBREV = "7b1e150"
SRC_URI[sha256sum] = "09165fe5df75ad665df304e89d494a02c5f379624604d230e1b595cb5ae3b5b8"

UNPACK_POSTFUNC = ""
UNPACK_POSTFUNC:ccimx93 = "copy_ele_a0_fw"
copy_ele_a0_fw() {
	cp -f ${WORKDIR}/mx93a0-ahab-container.img ${S}
}
do_unpack[postfuncs] += "${UNPACK_POSTFUNC}"

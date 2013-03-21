DESCRIPTION = "Firmware files for Digi's MXS based platforms, such as Atheros bluetooth."
SECTION = "base"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/LICENSE;md5=3f40d7994397109285ec7b81fdeb3b58"

PR = "r0"

FILESEXTRAPATHS_prepend := "${THISDIR}/firmware-${PV}:"
SRC_URI = "file://PS_ASIC.pst \
	   file://RamPatch.txt \
	   file://PS_ASIC_class_1.pst \
	   file://PS_ASIC_class_2.pst \
	   file://readme.txt \
	  "

do_install() {
    install -d ${D}/lib/firmware/ar3k/1020200/
    cp -rfv ${WORKDIR}/PS_ASIC.pst ${D}/lib/firmware/ar3k/1020200/
    cp -rfv ${WORKDIR}/RamPatch.txt ${D}/lib/firmware/ar3k/1020200/
    find ${D}/lib/firmware/ar3k/1020200/ -type f -exec chmod 644 '{}' ';'
}

PACKAGES += "${PN}-ar3k"

FILES_${PN}-ar3k  = "/lib/firmware/ar3k/*"

COMPATIBLE_MACHINE = "(mxs)"


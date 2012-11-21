SUMMARY = "Freescale IMX firmware"
DESCRIPTION = "Freescale IMX firmware such as for the VPU"
SECTION = "base"
LICENSE = "Freescale"
LIC_FILES_CHKSUM = "file://licenses/vpu/EULA;md5=6469a8514eed615d6bf8c09f41846d8f"
PR = "r4"

SRC_URI = "${DIGI_LOG_MIRROR}/firmware-imx-11.09.01.tar.gz \
	    file://vpu_fw_imx51.bin \
	    file://vpu_fw_imx53.bin \
           "

SRC_URI[md5sum] = "a629ddb53c06f582ef99445e50c8f75d"
SRC_URI[sha256sum] = "0061fb46a47fe1aa7e44099fe0e98e1ec7de68f91541c9cf867ffc6ca9ea691c"

COMPATIBLE_MACHINE = "(mx5)"

S = "${WORKDIR}/firmware-imx-11.09.01"

# todo: make FILES_ machine dependent using override, i.MX53 only now.
FILES_${PN} = "/lib/firmware/vpu/vpu_fw_imx53.bin"

# [DIGI] The linux-digi kernel does not use the sdma bin from user space
# [DIGI] It's hardcoded in sdma_code_mx51 and sdma_code_mx53
#FILES_${PN} += "/lib/firmware/sdma/sdma-imx53-to1.bin"

# [DIGI] These are not used by Digi hardware
#FILES_${PN} += "/lib/firmware/ath6k/*"
#FILES_${PN} += "/lib/firmware/ar3k/*"

do_patch() {
	bbwarn "workdir ${WORKDIR}"
	bbwarn "S ${S}"
	cp ${WORKDIR}/vpu_fw_imx51.bin ${S}/firmware/vpu/
	cp ${WORKDIR}/vpu_fw_imx53.bin ${S}/firmware/vpu/
}

do_install() {
	install -d ${D}/lib/firmware/vpu
	install -m 0755 ${S}/firmware/vpu/vpu_fw_imx53.bin ${D}/lib/firmware/vpu
        chmod 644 ${D}/lib/firmware/vpu/vpu_fw_imx53.bin
	install -d ${D}/lib/firmware/sdma
	install -m 0755 ${S}/firmware/sdma/sdma-imx53-to1.bin ${D}/lib/firmware/sdma
        chmod 644 ${D}/lib//firmware/sdma/sdma-imx53-to1.bin
	cp -rf ${S}/firmware/ath6k ${D}/lib/firmware
	cp -rf ${S}/firmware/ar3k ${D}/lib/firmware
}


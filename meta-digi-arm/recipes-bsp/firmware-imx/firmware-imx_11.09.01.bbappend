PR_append = "+digi.0"

SRC_URI = "${DIGI_LOG_MIRROR}/firmware-imx-11.09.01.tar.gz \
	    file://vpu_fw_imx51.bin \
	    file://vpu_fw_imx53.bin \
           "

SRC_URI[md5sum] = "a629ddb53c06f582ef99445e50c8f75d"
SRC_URI[sha256sum] = "0061fb46a47fe1aa7e44099fe0e98e1ec7de68f91541c9cf867ffc6ca9ea691c"

FILES_${PN} = ""
FILES_${PN} = "/lib/firmware/vpu/vpu_fw_imx53.bin"

# The linux-digi kernel does not use the sdma bin from user space
# It's hardcoded in sdma_code_mx51 and sdma_code_mx53
#FILES_${PN} += "/lib/firmware/sdma/sdma-imx53-to1.bin"

# These are not used by Digi hardware
#FILES_${PN} += "/lib/firmware/ath6k/*"
#FILES_${PN} += "/lib/firmware/ar3k/*"


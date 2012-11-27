PR_append = "+digi.0"

PLATFORM = "IMX51"

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}"
SRC_URI = "${DIGI_LOG_MIRROR}/${PN}-${PV}.tar.gz"
SRC_URI += " file://0001-ENGR00156800-vpu-Fix-decoding-mp4PackedPBFrame-strea.patch \
             file://0002-ENGR00162690-vpu-Fix-the-issue-of-rotation-180-degre.patch \
             file://imx-lib-11.09.01-0003-vpu-do-not-error-if-no-VPU-IRAM-present.patch "

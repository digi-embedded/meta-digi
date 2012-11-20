include imx-lib.inc

PR = "${INC_PR}.1"

COMPATIBLE_MACHINE = "(mx5)"

SRC_URI += " file://imx-lib-11.09.01-0001-ENGR00156800-vpu-Fix-decoding-mp4PackedPBFrame-strea.patch \
             file://imx-lib-11.09.01-0002-ENGR00162690-vpu-Fix-the-issue-of-rotation-180-degre.patch \
             file://imx-lib-11.09.01-0003-vpu-do-not-error-if-no-VPU-IRAM-present.patch "
SRC_URI[md5sum] = "45574f8f32f7000ca11d585fa60dea8c"
SRC_URI[sha256sum] = "f151a8bb3099b596b5834a1139c19e526802e6a0aa965018d16375e7e1f48f27"

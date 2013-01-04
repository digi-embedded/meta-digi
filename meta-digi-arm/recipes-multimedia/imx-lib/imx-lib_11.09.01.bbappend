FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}-${PV}:"
PR_append = "+del.r0"

SRC_URI += " file://imx-lib-11.09.01-0003-vpu-do-not-error-if-no-VPU-IRAM-present.patch "

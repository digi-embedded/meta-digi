DESCRIPTION = "DEL examples: V4L2 test application"
SECTION = "examples"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

PR = "r0"

RDEPENDS_${PN} = "kernel-module-mt9v111-camera"

SRC_URI = "file://v4l2_test"

S = "${WORKDIR}/v4l2_test"

do_compile() {
	${CC} -O2 -Wall v4l2_still.c -o v4l2_still -lpthread
	${CC} -O2 -Wall v4l2_common.c v4l2_preview_test.c -o v4l2_preview_test -lpthread
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 v4l2_still v4l2_preview_test ${D}${bindir}
}

PACKAGE_ARCH = "${MACHINE_ARCH}"

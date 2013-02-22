SUMMARY = "DEL examples: sahara driver test application"
SECTION = "examples"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

DEPENDS = "imx-lib"

RDEPENDS_${PN} = "kernel-module-scc2-driver kernel-module-sahara"

PR = "r0"

SRC_URI = "file://sahara_test"

SAHARA_TEST_SRC = "\
	apihelp.c \
	apitest.c \
	auth_decrypt.c \
	callback.c \
	cap.c \
	dryice.c \
	gen_encrypt.c \
	hash.c \
	hmac1.c \
	hmac2.c \
	rand.c \
	results.c \
	run_tests.c \
	smalloc.c \
	sym.c \
	user_wrap.c \
	wrap.c \
	"

S = "${WORKDIR}/sahara_test"

do_compile() {
	${CC} -O2 -Wall -DCONFIG_ARCH_MX5 -DSAHARA2 -DSAHARA \
		-I${STAGING_KERNEL_DIR}/drivers/mxc/security/sahara2/include \
		${SAHARA_TEST_SRC} -lsahara -o sahara_test
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 sahara_test ${D}${bindir}
}

PACKAGE_ARCH = "${MACHINE_ARCH}"

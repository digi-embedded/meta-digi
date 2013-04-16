# Copyright (C) 2013 Digi International.

DESCRIPTION = "Digi's NVRAM tool"
SECTION = "base"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

PR = "${DISTRO}.r0"

DEPENDS = "libdigi"

SRCREV  = "${AUTOREV}"
SRC_URI = " \
	${DIGI_LOG_GIT}u-boot-denx.git;protocol=git;branch=refs/heads/master \
	file://main.c \
	file://nvram_priv_linux.c \
	"

S = "${WORKDIR}"

CMD_GIT_SHA1 = "$(cd ${THISDIR} && git rev-parse --short HEAD)"
LIB_GIT_SHA1 = "$(cd ${WORKDIR}/git && git rev-parse --short HEAD)"

CFLAGS += "-Wall -DLINUX -DCMD_GIT_SHA1=\"${CMD_GIT_SHA1}\" -DLIB_GIT_SHA1=\"${LIB_GIT_SHA1}\" -Ilib/include -I${STAGING_INCDIR}/libdigi"

do_configure() {
	rm -f lib && ln -s git/common/digi/cmd_nvram/lib
}

do_compile() {
	# 'libnvram.a' static library
	${CC} ${CFLAGS} -c -o nvram.o lib/src/nvram.c
	${CC} ${CFLAGS} -c -o nvram_cmdline.o lib/src/nvram_cmdline.c
	${CC} ${CFLAGS} -c -o nvram_priv_linux.o nvram_priv_linux.c
	${AR} -rcs libnvram.a nvram.o nvram_cmdline.o nvram_priv_linux.o
	# 'nvram' command-line tool
	${CC} ${CFLAGS} -o nvram main.c libnvram.a -ldigi
}

do_install() {
	mkdir -p ${D}${base_sbindir} ${D}${includedir} ${D}${libdir}
	install -m 0644 libnvram.a ${D}${libdir}/
	install -m 0644 lib/include/nvram.h lib/include/nvram_types.h ${D}${includedir}/
	install -m 0755 nvram ${D}${base_sbindir}/
}

PACKAGE_ARCH = "${MACHINE_ARCH}"

# Copyright (C) 2013 Digi International.

SUMMARY = "Digi's NVRAM tool"
SECTION = "base"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

require recipes-bsp/u-boot/u-boot-dey-rev_${PREFERRED_VERSION_u-boot-dey}.inc

DEPENDS = "libdigi"

SRC_URI += " \
    file://main.c \
    file://nvram_priv_linux.c \
"

S = "${WORKDIR}"

CMD_GIT_SHA1 = "$(cd ${THISDIR} && git rev-parse --short=7 HEAD)"
LIB_GIT_SHA1 = "$(cd ${WORKDIR}/git && git rev-parse --short=7 HEAD)"

EXTRA_CFLAGS = "-Wall -DLINUX -DCMD_GIT_SHA1=\"${CMD_GIT_SHA1}\" -DLIB_GIT_SHA1=\"${LIB_GIT_SHA1}\" -Ilib/include -I${STAGING_INCDIR}/libdigi"

do_configure() {
	rm -f lib && ln -s ${UBOOT_NVRAM_LIBPATH}
}

do_compile() {
	# 'libnvram.a' static library
	${CC} ${CFLAGS} ${EXTRA_CFLAGS} -c -o nvram.o lib/src/nvram.c
	${CC} ${CFLAGS} ${EXTRA_CFLAGS} -c -o nvram_cmdline.o lib/src/nvram_cmdline.c
	${CC} ${CFLAGS} ${EXTRA_CFLAGS} -c -o nvram_priv_linux.o nvram_priv_linux.c
	${AR} -rcs libnvram.a nvram.o nvram_cmdline.o nvram_priv_linux.o
	# 'nvram' command-line tool
	${CC} ${CFLAGS} ${EXTRA_CFLAGS} -c -o main.o main.c
	${CC} ${LDFLAGS} -o nvram main.o libnvram.a -ldigi
}

do_install() {
	mkdir -p ${D}${base_sbindir} ${D}${includedir} ${D}${libdir}
	install -m 0644 libnvram.a ${D}${libdir}/
	install -m 0644 lib/include/nvram.h lib/include/nvram_types.h ${D}${includedir}/
	install -m 0755 nvram ${D}${base_sbindir}/
}

PACKAGE_ARCH = "${MACHINE_ARCH}"

# Copyright (C) 2013,2017 Digi International.

SUMMARY = "Digi's utilities library"
SECTION = "libs"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

SRC_URI = " \
    file://cmdopt.c \
    file://cmdopt.h \
    file://crc32.c \
    file://crc32.h \
    file://digi-platforms.h \
    file://log.c \
    file://log.h \
    file://mem.c \
    file://mem.h \
    file://misc_helper.h \
    file://platform.c \
"

S = "${WORKDIR}"

do_compile() {
	${CC} -O2 -Wall ${LDFLAGS} -c -o log.o log.c
	${CC} -O2 -Wall ${LDFLAGS} -c -o cmdopt.o cmdopt.c
	${CC} -O2 -Wall ${LDFLAGS} -c -o mem.o mem.c
	${CC} -O2 -Wall ${LDFLAGS} -c -o crc32.o crc32.c
	${CC} -O2 -Wall ${LDFLAGS} -c -o platform.o platform.c
	${AR} -rcs libdigi.a log.o cmdopt.o mem.o crc32.o platform.o
}

do_install() {
	mkdir -p ${D}${includedir}/libdigi ${D}${libdir}
	install -m 0644 libdigi.a ${D}${libdir}
	install -m 0644 cmdopt.h crc32.h digi-platforms.h log.h mem.h misc_helper.h ${D}${includedir}/libdigi
}

RDEPENDS_${PN}-dev = ""

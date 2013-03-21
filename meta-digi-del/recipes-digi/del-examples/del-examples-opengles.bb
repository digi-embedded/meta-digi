# Copyright (C) 2013 Digi International.

DESCRIPTION = "DEL examples: OpenGL-ES test application (based on Freescale GPU SDK)"
SECTION = "examples"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

DEPENDS = "virtual/egl virtual/libgles1 virtual/libgles2 virtual/kernel"

PR = "r0"

SRC_URI = "file://opengles"

S = "${WORKDIR}/opengles"

do_compile() {
	${CC} -O2 -Wall -Ilib/include es20_example.c lib/fslutil/fslutil.c -lm -lEGL -lGLESv2 -o es20_example
	${CC} -O2 -Wall -Ilib/include es11_example.c lib/fslutil/fslutil.c lib/glu3/glu3.c -lm -lEGL -lGLESv1_CM -o es11_example
}

do_install() {
	install -d ${D}${bindir} ${D}/usr/share/wallpapers
	install -m 0755 es20_example es11_example ${D}${bindir}
	install -m 0644 texture.bmp ${D}/usr/share/wallpapers
}

FILES_${PN} += "/usr/share/wallpapers/texture.bmp"

RDEPENDS_${PN} = "lib2dz160-mx51 lib2dz430-mx51"

PACKAGE_ARCH = "${MACHINE_ARCH}"

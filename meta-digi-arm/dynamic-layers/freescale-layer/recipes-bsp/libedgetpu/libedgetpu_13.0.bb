SUMMARY = "Edge TPU runtime library for Coral devices"
HOMEPAGE = "https://coral.googlesource.com/edgetpu"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://libedgetpu/LICENSE.txt;md5=c0e85c67b919e863a1a7a3da109dc40d"

SRC_URI = "https://dl.google.com/coral/edgetpu_api/edgetpu_runtime_20210119.zip"
SRC_URI[md5sum] = "5c0b992d73683e395d6993761064d2df"
SRC_URI[sha256sum] = "b23b2c5a227d7f0e65dcc91585028d27c12e764f8ce4c4db3f114be4a49af3ae"

S = "${WORKDIR}/edgetpu_runtime"

RDEPENDS:${PN} = "libusb1"

# The library files in direct correspond to max frequency, those in throttled correspond to reduced frequency.
LIBEDGETPU_TYPE = "direct"
LIBEDGETPU_ARCH = "aarch64"

do_install() {
    install -d ${D}${sysconfdir}/udev/rules.d
    install -m 0644 ${S}/libedgetpu/edgetpu-accelerator.rules \
                    ${D}${sysconfdir}/udev/rules.d/99-edgetpu-accelerator.rules

    install -d ${D}/${libdir}
    install -m 755 ${S}/libedgetpu/${LIBEDGETPU_TYPE}/${LIBEDGETPU_ARCH}/libedgetpu.so.1.0 \
                   ${D}/${libdir}/libedgetpu.so.1.0
    ln -sf ${libdir}/libedgetpu.so.1.0 ${D}/${libdir}/libedgetpu.so.1
    ln -sf ${libdir}/libedgetpu.so.1.0 ${D}/${libdir}/libedgetpu.so

    install -d ${D}/${includedir}
    install -m 755 ${S}/libedgetpu/edgetpu.h ${D}/${includedir}/edgetpu.h
}

FILES:${PN} += "${libdir}/libedgetpu.so \
                ${includedir}/edgetpu.h \
"

INSANE_SKIP:${PN} += "already-stripped"

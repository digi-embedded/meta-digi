# Copyright (C) 2023 Digi International.

SUMMARY = "STM key generation and signing tools"
SECTION = "console/tools"
LICENSE = "Proprietary"

LIC_FILES_CHKSUM = "file://en.SLA0048.txt;md5=108361d167deef887d204830aba9cf94"

PKGNAME = "trustfence-stm-signtools"

# tarball
SRC_URI = "${DIGI_PKG_SRC}/${PKGNAME}-${PV}.tar.gz"
SRC_URI[md5sum] = "5621d99156b75e69eecbae721475463b"
SRC_URI[sha256sum] = "5c04b16934aa316d2586980f93680e90af1968bcb9b76c18b9cecddbcf5d6a1b"

S = "${WORKDIR}/${PKGNAME}-${PV}"

inherit bin_package

INSANE_SKIP:${PN} = "already-stripped"

do_install() {
	install -d ${D}${bindir}
	install -m 0755 bin/* ${D}${bindir}
	install -d ${D}${base_libdir}
	install -m 0644 lib/* ${D}${base_libdir}
}

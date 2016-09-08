# Copyright (C) 2016 Digi International.

SRC_URI += "https://www.kernel.org/pub/software/network/wireless-regdb/wireless-regdb-2016.06.10.tar.xz;name=bin_2016_06_10"

SRC_URI[bin_2016_06_10.md5sum] = "d282cce92b6e692e8673e2bd97adf33b"
SRC_URI[bin_2016_06_10.sha256sum] = "cfedf1c3521b3c8f32602f25ed796e96e687c3441a00e7c050fedf7fd4f1b8b7"

do_install_append() {
	install -m 0644 ${WORKDIR}/wireless-regdb-2016.06.10/regulatory.bin ${D}${libdir}/crda/regulatory.bin
}

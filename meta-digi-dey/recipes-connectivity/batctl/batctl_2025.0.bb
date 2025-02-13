# Copyright (C) 2019-2025, Digi International Inc.
DESCRIPTION = "Control application for B.A.T.M.A.N. routing protocol kernel module for multi-hop ad-hoc mesh networks."
HOMEPAGE = "http://www.open-mesh.net/"
SECTION = "console/network"
PRIORITY = "optional"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

SRC_URI = "http://downloads.open-mesh.net/batman/stable/sources/batctl/batctl-${PV}.tar.gz"
SRC_URI[md5sum] = "a5413f2c1e7047192b773e95fbe17076"
SRC_URI[sha256sum] = "dd5f5b71dd1750eba8d69dd3adf70789741c6cf638cf2bf2ccbc2fb84f5c168f"

DEPENDS = "libnl"

inherit pkgconfig

do_install() {
  install -d ${D}${bindir}
  install -m 0755 batctl ${D}${bindir}
}

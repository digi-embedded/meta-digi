# Copyright (C) 2019 Digi International.
DESCRIPTION = "Control application for B.A.T.M.A.N. routing protocol kernel module for multi-hop ad-hoc mesh networks."
HOMEPAGE = "http://www.open-mesh.net/"
SECTION = "console/network"
PRIORITY = "optional"
LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

SRC_URI = "http://downloads.open-mesh.net/batman/stable/sources/batctl/batctl-${PV}.tar.gz"
SRC_URI[md5sum] = "3eb6c6e11d293f48d4ecab6d182805c2"
SRC_URI[sha256sum] = "997721096ff396644e8d697ea7651e9d38243faf317bcea2661d4139ff58b531"

DEPENDS = "libnl"

inherit pkgconfig

do_install() {
  install -d ${D}${bindir}
  install -m 0755 batctl ${D}${bindir}
}

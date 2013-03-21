DESCRIPTION = "nl80211 based CLI configuration utility for wireless devices"
DESCRIPTION = "iw is a new nl80211 based CLI configuration utility for \
wireless devices. It supports almost all new drivers that have been added \
to the kernel recently. "
HOMEPAGE = "http://linuxwireless.org/en/users/Documentation/iw"
SECTION = "base"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://COPYING;md5=878618a5c4af25e9b93ef0be1a93f774"

DEPENDS = "libnl pkgconfig"

PR = "${DISTRO}.r0"

SRC_URI = "http://www.kernel.org/pub/software/network/iw/iw-${PV}.tar.bz2 \
           file://0001-iw-version.sh-don-t-use-git-describe-for-versioning.patch \
          "

SRC_URI[md5sum] = "618ad1106a196fb1c3d827de96da437c"
SRC_URI[sha256sum] = "3dae92ca5989cbc21155941fa01907a5536da3c5f6898642440c61484fc7e0f9"

EXTRA_OEMAKE = ""

do_install() {
	oe_runmake DESTDIR=${D} install
}

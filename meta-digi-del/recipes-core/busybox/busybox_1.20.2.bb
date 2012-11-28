require recipes-core/busybox/busybox.inc

DEPENDS += "libdigi"
PR = "r1"

SRC_URI = "http://www.busybox.net/downloads/busybox-${PV}.tar.bz2;name=tarball \
           file://0001-del-baudrates.patch \
           file://0002-del-mdev_regulatory.patch \
           file://0003-del-flash_eraseall.patch \
           file://0004-kernel_ver.patch \
           file://defconfig \
           file://syslog \
           file://syslog-startup.conf \
           file://busybox-cron \
           file://busybox-httpd \
           file://busybox-udhcpd \
           file://hwclock.sh \
           file://simple.script \
           file://default.script \
           file://busybox-udhcpc \
           file://mdev \
           file://mdev.conf \
           "

SRC_URI[tarball.md5sum] = "e025414bc6cd79579cc7a32a45d3ae1c"
SRC_URI[tarball.sha256sum] = "eb13ff01dae5618ead2ef6f92ba879e9e0390f9583bd545d8789d27cf39b6882"

EXTRA_OEMAKE += "ARCH=${TARGET_ARCH} CROSS_COMPILE=${TARGET_PREFIX}"

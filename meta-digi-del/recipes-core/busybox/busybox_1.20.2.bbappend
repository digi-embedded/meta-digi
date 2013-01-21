FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}-${PV}:"
PR_append = "+del.r0"

DEPENDS += "libdigi"

SRC_URI += "file://0001-del-baudrates.patch \
            file://0002-del-mdev_regulatory.patch \
            file://0003-del-flash_eraseall.patch \
            file://defconfig \
            file://mdev.conf \
            file://adc \
            file://mmc \
            file://sd \
            file://ts \
            file://suspend \
           "

FILES_${PN}-mdev += "${base_libdir}/mdev/adc ${base_libdir}/mdev/mmc ${base_libdir}/mdev/sd ${base_libdir}/mdev/ts"

do_install_append() {
	if grep "CONFIG_MDEV=y" ${WORKDIR}/defconfig; then
		if grep "CONFIG_FEATURE_MDEV_CONF=y" ${WORKDIR}/defconfig; then
			install -d ${D}${base_libdir}/mdev
			install -m 0755 ${WORKDIR}/adc ${D}${base_libdir}/mdev/adc
			install -m 0755 ${WORKDIR}/mmc ${D}${base_libdir}/mdev/mmc
			install -m 0755 ${WORKDIR}/sd ${D}${base_libdir}/mdev/sd
			install -m 0755 ${WORKDIR}/ts ${D}${base_libdir}/mdev/ts
			ln -s ../lib/mdev/mmc ${D}${base_bindir}/mmc-mount
			ln -s ../lib/mdev/mmc ${D}${base_bindir}/mmc-umount
			ln -s ../lib/mdev/sd ${D}${base_bindir}/usbmount
			ln -s ../lib/mdev/sd ${D}${base_bindir}/usbumount
		fi
	fi
	# Install 'suspend' script
	install -m 0755 ${WORKDIR}/suspend ${D}${base_bindir}
}

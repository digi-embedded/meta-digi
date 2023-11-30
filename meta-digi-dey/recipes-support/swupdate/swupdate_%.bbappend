# Copyright (C) 2016-2023 Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

# Without libgcc, swupdate generates an error signal when terminating
RDEPENDS:${PN} += "libgcc"

SRC_URI += " \
    file://0001-Makefile-change-Makefile-to-build-swupdate-library-s.patch \
    file://0002-config-add-on-the-fly-build-configuration-variable.patch \
    file://0003-handlers-rdiff-handler-for-applying-librsync-s-rdiff.patch \
    ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'file://systemd.cfg', '', d)} \
    ${@bb.utils.contains('STORAGE_MEDIA', 'mtd', 'file://mtd.cfg', '', d)} \
    ${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', 'file://signed_images.cfg', '', d)} \
    file://swupdate.cfg \
"

do_install:append() {
	# Copy the 'progress' binary.
	install -d ${D}${bindir}/
	install -m 0755 tools/swupdate-progress ${D}${bindir}/progress

	# Copy config file
	install -d ${D}${sysconfdir}/
	install -m 0755 ${WORKDIR}/swupdate.cfg ${D}${sysconfdir}

	# Add MTD blacklist
	if ${@oe.utils.conditional('STORAGE_MEDIA', 'mtd', 'true', 'false', d)}; then
		sed -i "s,\(^\s*\)#mtd-blacklist,\1mtd-blacklist = \"${SWUPDATE_MTD_BLACKLIST}\",g" ${D}${sysconfdir}/swupdate.cfg
	fi

	# Add public-key-file setting to config file if TrustFence is enabled
	if ${@oe.utils.conditional('TRUSTFENCE_ENABLED', '1', 'true', 'false', d)}; then
		sed -i "s,\(^\s*\)#public-key-file,\1public-key-file = \"${sysconfdir}/ssl/certs/key.pub\",g" ${D}${sysconfdir}/swupdate.cfg
	fi
}

pkg_postinst_ontarget:${PN}() {
	if ${@bb.utils.contains('DISTRO_FEATURES','systemd','false','true',d)}; then
		[ "$(fw_printenv -n dualboot 2>/dev/null)" != "yes" ] && update-rc.d -f swupdate remove
	fi
}

inherit ${@bb.utils.contains("IMAGE_FEATURES", "read-only-rootfs", "remove-pkg-postinst-ontarget", "", d)}

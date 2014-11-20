# Copyright (C) 2013 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += " \
    file://os-release \
    file://sysctl.conf \
"

SRC_URI_append_ccimx6 = " file://resize-ext4fs.sh"

do_install_append() {
	install -m 0644 ${WORKDIR}/os-release ${D}${sysconfdir}/
	sed -i  -e 's,##DISTRO##,${DISTRO},g' \
		-e 's,##DISTRO_NAME##,${DISTRO_NAME},g' \
		-e 's,##DISTRO_VERSION##,${DISTRO_VERSION},g' \
		${D}${sysconfdir}/os-release
	install -m 0644 ${WORKDIR}/sysctl.conf ${D}${sysconfdir}/
}

do_install_append_mx5() {
	cat >> ${D}${sysconfdir}/sysctl.conf <<-EOF
		# Protect the DMA zone and avoid memory allocation error
		vm.lowmem_reserve_ratio = 1 1
	EOF
}

do_install_append_ccimx6() {
	install -d ${D}${sysconfdir}/${IMAGE_PKGTYPE}-postinsts
	install -m 0755 ${WORKDIR}/resize-ext4fs.sh ${D}${sysconfdir}/${IMAGE_PKGTYPE}-postinsts/
}

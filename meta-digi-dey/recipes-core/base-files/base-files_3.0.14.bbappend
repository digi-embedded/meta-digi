# Copyright (C) 2013 Digi International.

PRINC := "${@int(PRINC) + 1}"
PR_append = "+${DISTRO}"

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += " \
    file://os-release \
    file://sysctl.conf \
"

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

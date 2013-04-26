# Copyright (C) 2013 Digi International.

PRINC := "${@int(PRINC) + 1}"
PR_append = "+${DISTRO}"

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += " \
	file://os-release \
	file://sysctl.conf \
"

TIMESTAMP  = "${@time.strftime('%Y%m%d%H%M')}"
LAYERS_REV = "${@"\nLayers revisions:\n%s\n" % '\n'.join(get_layers_branch_rev(d))}"
DEL_TAG    = "${@del_tag(d).strip()}"

def del_tag(d):
    import subprocess
    for layer in d.getVar('BBLAYERS', True).split():
        if 'meta-digi-del' in layer:
            cmd = 'git describe --tags --exact-match 2>/dev/null || true'
            return subprocess.Popen(cmd, cwd=layer, shell=True, stdout=subprocess.PIPE).stdout.read()
    return ""

do_install_append() {
	install -m 0644 ${WORKDIR}/os-release ${D}${sysconfdir}/
	sed -i -e 's,##DEL_TAG##,${DEL_TAG},g' ${D}${sysconfdir}/os-release
	sed -i -e 's,##BUILD_TIMESTAMP##,${TIMESTAMP},g' ${D}${sysconfdir}/os-release
	cat >> ${D}${sysconfdir}/os-release <<-EOF
		${LAYERS_REV}
	EOF
	install -m 0644 ${WORKDIR}/sysctl.conf ${D}${sysconfdir}/
}

do_install_append_mx5() {
	cat >> ${D}${sysconfdir}/sysctl.conf <<-EOF
		# Protect the DMA zone and avoid memory allocation error
		vm.lowmem_reserve_ratio = 1 1
	EOF
}

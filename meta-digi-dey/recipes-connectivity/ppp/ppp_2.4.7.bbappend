FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}-${PV}:"

SRC_URI += "file://mm_cellular"

do_install_append () {
        mkdir -p ${D}${sysconfdir}/ppp/peers
        install -m 0755 ${WORKDIR}/mm_cellular ${D}${sysconfdir}/ppp/peers/mm_cellular
}

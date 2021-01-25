require recipes-support/dnsmasq/dnsmasq.inc

# To avoid duplicating files, re-use the original ones from meta-openembedded.
FILESEXTRAPATHS_prepend := "${COREBASE}/../meta-openembedded/meta-networking/recipes-support/${PN}/files:"

SRC_URI[dnsmasq-2.83.md5sum] = "c87d5af020d12984d2ab9fbf04e2dcca"
SRC_URI[dnsmasq-2.83.sha256sum] = "6b67955873acc931bfff61a0a1e0dc239f8b52e31df50e9164d3a4537571342f"
SRC_URI += "\
    file://dnsmasq-resolved.conf \
    file://lua.patch \
"

do_install_append() {
	install -d ${D}${sysconfdir}/systemd/resolved.conf.d/
	install -m 0644 ${WORKDIR}/dnsmasq-resolved.conf ${D}${sysconfdir}/systemd/resolved.conf.d/
}

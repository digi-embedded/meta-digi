SUMMARY = "NXP Code signing Tool for the High Assurance Boot library"
DESCRIPTION = "Provides software code signing support designed for use with i.MX processors that integrate the HAB library in the internal boot ROM."
HOMEPAGE = "https://www.nxp.com/webapp/Download?colCode=IMX_CST_TOOL"
LICENSE = "CLOSED"

DEPENDS = "openssl-native"

S = "${WORKDIR}/cst-${PV}"

inherit native

SRC_URI = " \
	${@base_conditional('TRUSTFENCE_SIGN', '1', 'file://cst-${PV}.tar.gz', '', d)} \
	file://0001-gen_auth_encrypted_data-reuse-existing-DEK-file.patch \
	file://0002-hab4_pki_tree.sh-automate-script.patch \
	file://0003-openssl_helper-use-dev-urandom-as-seed-source.patch \
	file://Makefile \
"

do_configure() {
	cp -f ${WORKDIR}/Makefile .
}

do_compile () {
	oe_runmake clean && oe_runmake
}

do_install () {
	install -d ${D}${bindir}
	install -m 0755 linux64/cst ${D}${bindir}/cst
	install -m 0755 linux64/srktool ${D}${bindir}/srktool
	install -m 0755 keys/hab4_pki_tree.sh ${D}${bindir}/trustfence-gen-pki.sh
	install -m 0755 ca/openssl.cnf ${D}${bindir}/openssl.cnf
	install -m 0755 ca/v3_ca.cnf ${D}${bindir}/v3_ca.cnf
	install -m 0755 ca/v3_usr.cnf ${D}${bindir}/v3_usr.cnf
}

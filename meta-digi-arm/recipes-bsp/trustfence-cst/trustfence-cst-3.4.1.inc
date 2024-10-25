# Copyright (C) 2017-2024, Digi International Inc.

SUMMARY = "NXP Code signing Tool for the High Assurance Boot library"
DESCRIPTION = "Provides software code signing support designed for use with \
i.MX processors that integrate the HAB library in the internal boot ROM."
HOMEPAGE = "https://www.nxp.com/webapp/Download?colCode=IMX_CST_TOOL"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://LICENSE.bsd3;md5=14aba05f9fa6c25527297c8aac95fcf6"

DEPENDS = "byacc-native flex-native openssl"

SRC_URI = " \
    ${DIGI_PKG_SRC}/cst-${PV}.tgz;name=cst \
    file://0001-gen_auth_encrypted_data-reuse-existing-DEK-file.patch \
    file://0002-openssl_helper-use-dev-urandom-as-seed-source.patch \
    file://0003-hab4_pki_tree.sh-adapt-script-for-DEY.patch \
    file://0004-ahab_pki_tree.sh-adapt-script-for-DEY.patch \
"

SRC_URI[cst.md5sum] = "b23ed5983734d4812fcf1da33eac8f31"
SRC_URI[cst.sha256sum] = "0715fffe43d82708696af3f5d721bf36edd1d9be1ec482f21b9f1c48bc699097"

S = "${WORKDIR}/cst-${PV}"

EXTRA_OEMAKE = 'CC="${CC}" LD="${CC}" AR="${AR}" OBJCOPY="${OBJCOPY}"'

do_compile() {
	oe_runmake -C code/obj.linux64 OSTYPE=linux64 ENCRYPTION=yes COPTIONS="${CFLAGS} ${CPPFLAGS}" LDOPTIONS="${LDFLAGS}"
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 code/obj.linux64/cst ${D}${bindir}
	install -m 0755 code/obj.linux64/mac_dump ${D}${bindir}
	install -m 0755 code/obj.linux64/srktool ${D}${bindir}
	if [ "${TRUSTFENCE_SIGN_MODE}" = "AHAB" ]; then
		install -m 0755 keys/ahab_pki_tree.sh ${D}${bindir}/trustfence-gen-pki.sh
	elif [ "${TRUSTFENCE_SIGN_MODE}" = "HAB" ]; then
		install -m 0755 keys/hab4_pki_tree.sh ${D}${bindir}/trustfence-gen-pki.sh
	else
		bberror "Unkown TRUSTFENCE_SIGN_MODE value"
		exit 1
	fi
	install -m 0755 ca/openssl.cnf ${D}${bindir}/openssl.cnf
	install -m 0755 ca/v3_ca.cnf ${D}${bindir}/v3_ca.cnf
	install -m 0755 ca/v3_usr.cnf ${D}${bindir}/v3_usr.cnf
}

FILES:${PN} = "${bindir}"

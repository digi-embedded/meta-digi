# Copyright (C) 2017-2023 Digi International

SUMMARY = "NXP Code signing Tool for the High Assurance Boot library"
DESCRIPTION = "Provides software code signing support designed for use with \
i.MX processors that integrate the HAB library in the internal boot ROM."
HOMEPAGE = "https://www.nxp.com/webapp/Download?colCode=IMX_CST_TOOL"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://LICENSE.bsd3;md5=1fbcd66ae51447aa94da10cbf6271530"

DEPENDS = "byacc-native flex-native"

OPENSSL1_VERSION = "1.1.1t"

SRC_URI = " \
    ${DIGI_PKG_SRC}/cst-${PV}.tgz;name=cst \
    https://www.openssl.org/source/openssl-${OPENSSL1_VERSION}.tar.gz;name=openssl \
    file://0001-gen_auth_encrypted_data-reuse-existing-DEK-file.patch \
    file://0002-openssl_helper-use-dev-urandom-as-seed-source.patch \
    file://0003-hab4_pki_tree.sh-adapt-script-for-DEY.patch \
    file://0004-ahab_pki_tree.sh-adapt-script-for-DEY.patch \
"

SRC_URI[cst.md5sum] = "4b9fccac381fa412cba8ba7028c154c7"
SRC_URI[cst.sha256sum] = "517b11dca181e8c438a6249f56f0a13a0eb251b30e690760be3bf6191ee06c68"
SRC_URI[openssl.md5sum] = "1cfee919e0eac6be62c88c5ae8bcd91e"
SRC_URI[openssl.sha256sum] = "8dee9b24bdb1dcbf0c3d1e9b02fb8f6bf22165e807f45adeb7c9677536859d3b"

S = "${WORKDIR}/cst-${PV}"

do_compile() {
	cd code/cst
	oe_runmake OPENSSL_PATH=${WORKDIR}/openssl-${OPENSSL1_VERSION} OSTYPE=linux64 openssl
	oe_runmake OPENSSL_PATH=${WORKDIR}/openssl-${OPENSSL1_VERSION} OSTYPE=linux64 os_bin
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 code/cst/code/obj.linux64/cst ${D}${bindir}
	install -m 0755 code/cst/code/obj.linux64/srktool ${D}${bindir}
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

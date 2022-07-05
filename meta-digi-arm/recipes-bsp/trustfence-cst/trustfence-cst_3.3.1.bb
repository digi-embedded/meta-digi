# Copyright (C) 2017-2020 Digi International
SUMMARY = "NXP Code signing Tool for the High Assurance Boot library"
DESCRIPTION = "Provides software code signing support designed for use with \
i.MX processors that integrate the HAB library in the internal boot ROM."
HOMEPAGE = "https://www.nxp.com/webapp/Download?colCode=IMX_CST_TOOL"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://LICENSE.bsd3;md5=1fbcd66ae51447aa94da10cbf6271530"

DEPENDS = "byacc flex"

# Explicitly add byacc-native as a dependency when building the package for the
# SDK, otherwise, it won't get installed in the sysroot, causing a compilation
# error.
# Explicitly add openssl-native for the SDK build to correctly link to the
# openssl libraries in the native dependencies folder.
DEPENDS:append:class-nativesdk = " byacc-native"

SRC_URI = " \
    ${DIGI_PKG_SRC}/cst-${PV}.tgz;name=cst \
    https://www.openssl.org/source/openssl-1.1.1q.tar.gz;name=openssl \
    file://0001-gen_auth_encrypted_data-reuse-existing-DEK-file.patch \
    file://0002-hab4_pki_tree.sh-automate-script.patch \
    file://0003-openssl_helper-use-dev-urandom-as-seed-source.patch \
    file://0004-hab4_pki_tree.sh-usa-a-random-password-for-the-defau.patch \
    file://0005-ahab_pki_tree.sh-automate-script.patch \
    file://0006-ahab_pki_tree.sh-use-a-random-password-for-the-defau.patch \
"

SRC_URI[cst.md5sum] = "27ba9c8bc0b8a7f14d23185775c53794"
SRC_URI[cst.sha256sum] = "8b7e44e3e126f814f5caf8a634646fe64021405302ca59ff02f5c8f3b9a5abb9"
SRC_URI[openssl.md5sum] = "c685d239b6a6e1bd78be45624c092f51"
SRC_URI[openssl.sha256sum] = "d7939ce614029cdff0b6c20f0e2e5703158a489a72b2507b8bd51bf8c8fd10ca"

S = "${WORKDIR}/cst-${PV}"

do_compile() {
	cd code/cst
	oe_runmake OPENSSL_PATH=${WORKDIR}/openssl-1.1.1q OSTYPE=linux64 openssl
	oe_runmake OPENSSL_PATH=${WORKDIR}/openssl-1.1.1q OSTYPE=linux64 rel_bin
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
BBCLASSEXTEND = "native nativesdk"

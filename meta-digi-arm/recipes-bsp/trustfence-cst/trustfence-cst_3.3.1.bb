# Copyright (C) 2017-2020 Digi International
SUMMARY = "NXP Code signing Tool for the High Assurance Boot library"
DESCRIPTION = "Provides software code signing support designed for use with \
i.MX processors that integrate the HAB library in the internal boot ROM."
HOMEPAGE = "https://www.nxp.com/webapp/Download?colCode=IMX_CST_TOOL"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://LICENSE.bsd3;md5=1fbcd66ae51447aa94da10cbf6271530"

DEPENDS = "openssl byacc flex"

# Explicitly add byacc-native as a dependency when building the package for the
# SDK, otherwise, it won't get installed in the sysroot, causing a compilation
# error.
# Explicitly add openssl-native for the SDK build to correctly link to the
# openssl libraries in the native dependencies folder.
DEPENDS_append_class-nativesdk = " byacc-native openssl-native"

SRC_URI = " \
    ${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', '${DIGI_PKG_SRC}/cst-${PV}.tgz', '', d)} \
    file://0001-gen_auth_encrypted_data-reuse-existing-DEK-file.patch \
    file://0002-hab4_pki_tree.sh-automate-script.patch \
    file://0003-openssl_helper-use-dev-urandom-as-seed-source.patch \
    file://0004-hab4_pki_tree.sh-usa-a-random-password-for-the-defau.patch \
    file://0005-ahab_pki_tree.sh-automate-script.patch \
    file://0006-ahab_pki_tree.sh-use-a-random-password-for-the-defau.patch \
    file://0007-Makefile-statically-link-libcrypto.patch \
"

SRC_URI[md5sum] = "27ba9c8bc0b8a7f14d23185775c53794"
SRC_URI[sha256sum] = "8b7e44e3e126f814f5caf8a634646fe64021405302ca59ff02f5c8f3b9a5abb9"

S = "${WORKDIR}/cst-${PV}/"

do_compile() {
	export LDLIBPATH=-L${WORKDIR}/recipe-sysroot-native/usr/lib
	export COPTIONS=-I${WORKDIR}/recipe-sysroot-native/usr/include
	cd ${S}/code/cst
	oe_runmake OSTYPE=linux64 clean
	oe_runmake OSTYPE=linux64 rel_bin
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 $(find ${S}/code/cst/release/linux64 -type f -name cst) ${D}${bindir}/cst
	install -m 0755 $(find ${S}/code/cst/release/linux64 -type f -name srktool) ${D}${bindir}/srktool
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

INSANE_SKIP_${PN} += "already-stripped"

FILES_${PN} = "${bindir}"
BBCLASSEXTEND = "native nativesdk"

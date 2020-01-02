# Copyright (C) 2017-2019 Digi International
SUMMARY = "NXP Code signing Tool for the High Assurance Boot library"
DESCRIPTION = "Provides software code signing support designed for use with \
i.MX processors that integrate the HAB library in the internal boot ROM."
HOMEPAGE = "https://www.nxp.com/webapp/Download?colCode=IMX_CST_TOOL"
LICENSE = "CLOSED"

DEPENDS = "openssl"

SRC_URI = " \
    ${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', 'file://cst-${PV}.tgz', '', d)} \
    file://0001-gen_auth_encrypted_data-reuse-existing-DEK-file.patch \
    file://0002-hab4_pki_tree.sh-automate-script.patch \
    file://0003-openssl_helper-use-dev-urandom-as-seed-source.patch \
    file://0004-hab4_pki_tree.sh-usa-a-random-password-for-the-defau.patch \
    file://0005-ahab_pki_tree.sh-automate-script.patch \
    file://0006-ahab_pki_tree.sh-use-a-random-password-for-the-defau.patch \
    file://Makefile \
"

# Usually local files (with file:// protocol) are not checked for
# premirrors. But in this case we want to be able to download the 'cst'
# package from a premirror in case it's not already in the DL_DIR, so prepend
# a premirror for the 'file://' protocol.
python() {
    source_mirror_url = d.getVar('SOURCE_MIRROR_URL', True)
    if source_mirror_url:
        premirrors = d.getVar('PREMIRRORS', True)
        d.setVar('PREMIRRORS', "file://cst.* %s \\n %s" % (source_mirror_url, premirrors))
}

S = "${WORKDIR}/release"

do_configure() {
	cp -f ${WORKDIR}/Makefile .
}

do_compile() {
	oe_runmake clean && oe_runmake
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 linux64/cst ${D}${bindir}/cst
	install -m 0755 $(find linux64 -type f -name srktool) ${D}${bindir}/srktool
	if [ "${SIGN_MODE}" = "AHAB" ]; then
		install -m 0755 keys/ahab_pki_tree.sh ${D}${bindir}/trustfence-gen-pki.sh
	elif [ "${SIGN_MODE}" = "HAB" ]; then
		install -m 0755 keys/hab4_pki_tree.sh ${D}${bindir}/trustfence-gen-pki.sh
	else
		bberror "Unkown SIGN_MODE value"
		exit 1
	fi
	install -m 0755 ca/openssl.cnf ${D}${bindir}/openssl.cnf
	install -m 0755 ca/v3_ca.cnf ${D}${bindir}/v3_ca.cnf
	install -m 0755 ca/v3_usr.cnf ${D}${bindir}/v3_usr.cnf
}

INSANE_SKIP_${PN} += "already-stripped"

FILES_${PN} = "${bindir}"
BBCLASSEXTEND = "native nativesdk"

# Copyright (C) 2013 Digi International.

SUMMARY = "Atheros' wmiconfig proprietary tool"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Proprietary;md5=0557f9d92cf58f2ccdd50f62f8ac0b28"

DEPENDS = "libnl"

inherit pkgconfig

ATH_PROP_PN = "atheros-proprietary-v3.4p4-b3.4.0.158"

SRC_MD5    = "5693e26e904ee7b829bc09a202b5fdcb"
SRC_SHA256 = "9d29113a9832ee4960d75c42e0ba229c71ebfe1f1f6f7738b213329c6214e708"
BIN_MD5    = "f502062cdecae89cd68562e476a896c9"
BIN_SHA256 = "687b7139904b1de7424abfdf7968d30fe2cef3c9bae95638e05d9532aa2bca2b"

SRC_URI_src = " \
    http://build-linux.digi.com/yocto/downloads/${ATH_PROP_PN}.tar.gz;md5sum=${SRC_MD5};sha256sum=${SRC_SHA256} \
    file://0001-cross_compile.patch \
"
SRC_URI_bin = "file://wmiconfig;md5sum=${BIN_MD5};sha256sum=${BIN_SHA256}"
SRC_URI = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRC_URI_src}', '${SRC_URI_bin}', d)}"

S = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${WORKDIR}/${ATH_PROP_PN}', '${WORKDIR}', d)}"

EXTRA_OEMAKE = ""

do_compile() {
	if [ "${DIGI_INTERNAL_GIT}" = "1" ]; then
		oe_runmake -C libtcmd
		oe_runmake -C ath6kl-wmiconfig
	fi
}

do_install() {
	install -d ${D}${sbindir}
	if [ "${DIGI_INTERNAL_GIT}" = "1" ]; then
		install -m 0755 ath6kl-wmiconfig/wmiconfig ${D}${sbindir}
	else
		install -m 0755 wmiconfig ${D}${sbindir}
	fi
}

# Deploy binary if building from sources
do_deploy() {
	if [ "${DIGI_INTERNAL_GIT}" = "1" ]; then
		install -d ${DEPLOY_DIR_IMAGE}
		if [ -f "${D}${sbindir}/wmiconfig" ]; then
			cp "${D}${sbindir}/wmiconfig" ${DEPLOY_DIR_IMAGE}/
		else
			bberror "Wmiconfig binary not found: "${D}${sbindir}/wmiconfig""
			exit 1
		fi
	fi
}
addtask deploy before do_build after do_install

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "mxs"

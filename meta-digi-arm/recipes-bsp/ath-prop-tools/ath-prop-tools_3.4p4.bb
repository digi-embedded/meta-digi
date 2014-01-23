# Copyright (C) 2013 Digi International.

SUMMARY = "Atheros' proprietary tools"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Proprietary;md5=0557f9d92cf58f2ccdd50f62f8ac0b28"

DEPENDS = "libnl"

inherit pkgconfig

ATH_PROP_PN = "atheros-proprietary-v3.4p4-b3.4.0.158"

SRC_URI_src = " \
    http://build-linux.digi.com/yocto/downloads/${ATH_PROP_PN}.tar.gz;name=tarball \
    file://0001-cross-compile.patch \
"
SRC_URI_bin = " \
    file://athtestcmd;name=athtestcmd \
    file://wmiconfig;name=wmiconfig \
"
SRC_URI = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRC_URI_src}', '${SRC_URI_bin}', d)}"

SRC_URI[tarball.md5sum] = "5693e26e904ee7b829bc09a202b5fdcb"
SRC_URI[tarball.sha256sum] = "9d29113a9832ee4960d75c42e0ba229c71ebfe1f1f6f7738b213329c6214e708"

SRC_URI[athtestcmd.md5sum] = "1ef34615e67692ba68aeeab6bf665d7c"
SRC_URI[athtestcmd.sha256sum] = "195c32fc54ec336a7daed165929427addb53605f7cf1709478b89704efb84269"
SRC_URI[wmiconfig.md5sum] = "f502062cdecae89cd68562e476a896c9"
SRC_URI[wmiconfig.sha256sum] = "687b7139904b1de7424abfdf7968d30fe2cef3c9bae95638e05d9532aa2bca2b"

S = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${WORKDIR}/${ATH_PROP_PN}', '${WORKDIR}', d)}"

EXTRA_OEMAKE = ""

do_compile() {
	if [ "${DIGI_INTERNAL_GIT}" = "1" ]; then
		oe_runmake -C libtcmd
		oe_runmake -C ath6kl-tcmd
		oe_runmake -C ath6kl-wmiconfig
	fi
}

do_install() {
	install -d ${D}${sbindir}
	if [ "${DIGI_INTERNAL_GIT}" = "1" ]; then
		install -m 0755 ath6kl-tcmd/athtestcmd ${D}${sbindir}
		install -m 0755 ath6kl-wmiconfig/wmiconfig ${D}${sbindir}
	else
		install -m 0755 athtestcmd wmiconfig ${D}${sbindir}
	fi
}

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "mxs"

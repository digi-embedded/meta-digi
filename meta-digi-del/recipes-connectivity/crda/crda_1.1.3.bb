# Copyright (C) 2013 Digi International.

DESCRIPTION = "Wireless Central Regulatory Domain Agent"
HOMEPAGE = "http://wireless.kernel.org/en/developers/Regulatory/CRDA"
SECTION = "base"
PRIORITY = "optional"
LIC_FILES_CHKSUM = "file://LICENSE;md5=07c4f6dea3845b02a18dc00c8c87699c"
LICENSE = "ISC"

PR = "${DISTRO}.r0"

# Virtual runtime settings can be overriden by the distribution.
VIRTUAL-RUNTIME_crda_use_gcrypt ?= "0"

# The distribution could set this to its device manger, for example udev.
VIRTUAL-RUNTIME_device_manager ?= "busybox-mdev"

DEPENDS = "libnl ${@base_conditional('VIRTUAL-RUNTIME_crda_use_gcrypt', '1' , 'libgcrypt python-m2crypto-native python-native', '', d)}"
RDEPENDS_${PN} = '${VIRTUAL-RUNTIME_device_manager}'

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI = "http://wireless.kernel.org/download/crda/${PN}-${PV}.tar.bz2;name=crda \
	   http://wireless.kernel.org/download/wireless-regdb/regulatory.bins/2011.04.28-regulatory.bin;name=reg \
	  "

SRC_URI += '${@base_conditional("VIRTUAL-RUNTIME_crda_use_gcrypt", "1" , "", "file://0001-Make-crypto-optional.patch", d)}'

EXTRA_OEMAKE = "MAKEFLAGS="
do_compile() {
	oe_runmake all_noverify
}

do_install() {
	oe_runmake DESTDIR=${D} install
	install -d ${D}/usr/lib/crda/
	install -m 0644 ${WORKDIR}/2011.04.28-regulatory.bin ${D}/usr/lib/crda/regulatory.bin
}

SRC_URI[crda.md5sum] = "29579185e06a75675507527243d28e5c"
SRC_URI[crda.sha256sum] = "aa8a7fe92f0765986c421a5b6768a185375ac210393df0605ee132f6754825f0"
SRC_URI[reg.md5sum] = "1535e98bcaba732e2f8e8f62dac6f369"
SRC_URI[reg.sha256sum] = "bb6ba6f5dcdf7106a19c588b0e4d43ab7af26f6474fe01011a318b3dceaba33b"

FILES_${PN} += "\
	/lib/udev/rules.d/85-regulatory.rules \
	/usr/lib/crda/regulatory.bin \
	"

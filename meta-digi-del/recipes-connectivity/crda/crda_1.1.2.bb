DESCRIPTION = "Wireless Central Regulatory Domain Agent"
HOMEPAGE = "http://wireless.kernel.org/en/developers/Regulatory/CRDA"
SECTION = "base"
PRIORITY = "optional"
LIC_FILES_CHKSUM = "file://LICENSE;md5=07c4f6dea3845b02a18dc00c8c87699c"
LICENSE = "ISC"
PR = "+del.r0"

# Virtual runtime settings can be overriden by the distribution.
VIRTUAL-RUNTIME_crda_use_gcrypt ?= "0"

# The distribution could set this to its device manger, for example udev.
VIRTUAL-RUNTIME_device_manager ?= "busybox-mdev"

DEPENDS = "libnl ${@base_conditional('VIRTUAL-RUNTIME_crda_use_gcrypt', '1' , 'libgcrypt python-m2crypto-native python-native', '', d)}"
RDEPENDS_${PN} = '${VIRTUAL-RUNTIME_device_manager}'

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

# Original SRC_URIs
#	http://wireless.kernel.org/download/crda/${P}.tar.bz2;name=crda \
#	http://wireless.kernel.org/download/wireless-regdb/regulatory.bins/2011.04.28-regulatory.bin;name=reg

SRC_URI = "${DIGI_LOG_MIRROR}${PN}-${PV}.tar.bz2;name=crda \
	   ${DIGI_LOG_MIRROR}2011.04.28-regulatory.bin;name=reg \
	  "

SRC_URI += '${@base_conditional("VIRTUAL-RUNTIME_crda_use_gcrypt", "1" , "", "file://0001-Make-crypto-optional.patch", d)}'

CFLAGS += " -DCONFIG_LIBNL20"

EXTRA_OEMAKE = "MAKEFLAGS="
do_compile() {
	oe_runmake all_noverify
}

do_install() {
	oe_runmake DESTDIR=${D} install
	install -d ${D}/usr/lib/crda/
	install -m 0644 ${WORKDIR}/2011.04.28-regulatory.bin ${D}/usr/lib/crda/regulatory.bin
}

SRC_URI[crda.md5sum] = "5226f65aebacf94baaf820f8b4e06df4"
SRC_URI[crda.sha256sum] = "e469348a5d0bb933df31995869130f68901de9be02e666437f52125698851864"
SRC_URI[reg.md5sum] = "1535e98bcaba732e2f8e8f62dac6f369"
SRC_URI[reg.sha256sum] = "bb6ba6f5dcdf7106a19c588b0e4d43ab7af26f6474fe01011a318b3dceaba33b"

FILES_${PN} += "\
	/lib/udev/rules.d/85-regulatory.rules \
	/usr/lib/crda/regulatory.bin \
	"

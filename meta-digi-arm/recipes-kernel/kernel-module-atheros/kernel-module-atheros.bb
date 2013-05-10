# Copyright (C) 2013 Digi International.

SUMMARY = "Atheros's wireless driver"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/ISC;md5=f3b90e78ea0cffb20bf5cca7947a896d"

inherit module

PR = "r0"

# Uncomment to build the driver from GIT repository (internal use only)
# ATHEROS_BUILD_FROM_GIT ?= "1"

SRCREV = "15bae2c4e330ea6d9289217d3c38ebf63aa8ff15"
SRCREV_SHORT = "${@'${SRCREV}'[:7]}"

# Checksums for 'atheros-${SRCREV_SHORT}.tar.gz' tarball
TARBALL_MD5    = "bd6b75eac4b5041f1a9272527ee6e2ee"
TARBALL_SHA256 = "66396f801b6d4254370d81ebaa34f963207dfcf2f9ce4ec72a86f11906ec9b0e"

SRC_URI_git = "${DIGI_LOG_GIT}linux-modules/atheros.git;protocol=git"
SRC_URI_tar = "${DIGI_MIRROR}/atheros-${SRCREV_SHORT}.tar.gz;md5sum=${TARBALL_MD5};sha256sum=${TARBALL_SHA256}"

SRC_URI  = "${@base_conditional('ATHEROS_BUILD_FROM_GIT', '1' , '${SRC_URI_git}', '${SRC_URI_tar}', d)}"
SRC_URI += " \
	file://atheros \
	file://atheros.conf \
	file://Makefile \
	"

S = "${@base_conditional('ATHEROS_BUILD_FROM_GIT', '1' , '${WORKDIR}/git', '${WORKDIR}/atheros-${SRCREV_SHORT}', d)}"

EXTRA_OEMAKE = "DEL_PLATFORM=${MACHINE} KLIB_BUILD=${STAGING_KERNEL_DIR}"

do_configure_prepend() {
	cp ${WORKDIR}/Makefile ${S}/Makefile
}

do_install_append() {
	install -d ${D}${sysconfdir}/network/if-pre-up.d
	install -m 0755 ${WORKDIR}/atheros ${D}${sysconfdir}/network/if-pre-up.d/
	install -d ${D}${sysconfdir}/modprobe.d
	install -m 0644 ${WORKDIR}/atheros.conf ${D}${sysconfdir}/modprobe.d/
}

FILES_${PN} += " \
	/lib/firmware/ath6k/AR6003/hw2.1.1/athtcmd_ram.bin \
	/lib/firmware/ath6k/AR6003/hw2.1.1/athwlan.bin \
	/lib/firmware/ath6k/AR6003/hw2.1.1/Digi_6203-6233-US.bin \
	/lib/firmware/ath6k/AR6003/hw2.1.1/Digi_6203-6233-World.bin \
	/lib/firmware/ath6k/AR6003/hw2.1.1/fw-4.bin \
	/lib/firmware/ath6k/AR6003/hw2.1.1/nullTestFlow.bin \
	/lib/firmware/ath6k/AR6003/hw2.1.1/utf.bin \
	"
FILES_${PN}_append_cpx2 = " \
	/lib/firmware/ath6k/AR6003/hw2.1.1/calData_AR6103_Digi_X2e_B.bin \
	/lib/firmware/ath6k/AR6003/hw2.1.1/calData_AR6103_Digi_X2e_B_world.bin \
	"

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(ccardimx28js|cpx2)"

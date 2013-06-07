# Copyright (C) 2013 Digi International.

SUMMARY = "Atheros's wireless driver"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/ISC;md5=f3b90e78ea0cffb20bf5cca7947a896d"

inherit module

PR = "r0"

SRCREV_external = "9b21b4508d08a2fb3bf3c55aaac182bb9c1210f2"
SRCREV_internal = "15bae2c4e330ea6d9289217d3c38ebf63aa8ff15"
SRCREV = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRCREV_internal}', '${SRCREV_external}', d)}"

SRC_URI_external = "${DIGI_GITHUB_GIT}/atheros.git;protocol=git"
SRC_URI_internal = "${DIGI_LOG_GIT}linux-modules/atheros.git;protocol=git"
SRC_URI = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRC_URI_internal}', '${SRC_URI_external}', d)}"
SRC_URI += " \
	file://atheros \
	file://atheros.conf \
	file://Makefile \
	"

S = "${WORKDIR}/git"

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

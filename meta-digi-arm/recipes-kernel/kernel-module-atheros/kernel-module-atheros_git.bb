# Copyright (C) 2013 Digi International.

DESCRIPTION = "Atheros's wireless driver"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/BSD;md5=3775480a712fc46a69647678acb234cb"

inherit module

PR = "r0"

# Uncomment to build the driver from sources (internal use only)
# ATHEROS_BUILD_SRC ?= "1"

SRCREV = "e135bedca602cdcf25f4f48a6aafeb7311f7c700"

# Checksums for 'atheros-${MACHINE}-${SRCREV_SHORT}.tar.gz' tarballs
TARBALL_MD5_ccardimx28js    = "75d1ee875ca686927f62cb004a26911b"
TARBALL_SHA256_ccardimx28js = "b975ede2f28e5a54433e25fd9ea35556d7ddc1382d26805f33af068110a18395"
TARBALL_MD5_cpx2            = ""
TARBALL_SHA256_cpx2         = ""

SRC_URI_git = " \
	${DIGI_LOG_GIT}linux-modules/atheros.git;protocol=git \
	file://Makefile.git \
	"
SRCREV_SHORT = "${@'${SRCREV}'[:7]}"
SRC_URI_obj = " \
	${DIGI_MIRROR}/atheros-${MACHINE}-${SRCREV_SHORT}.tar.gz;md5sum=${TARBALL_MD5};sha256sum=${TARBALL_SHA256} \
	file://Makefile.obj \
	"

SRC_URI  = "${@base_conditional('ATHEROS_BUILD_SRC', '1' , '${SRC_URI_git}', '${SRC_URI_obj}', d)}"
SRC_URI += " \
	file://atheros \
	file://atheros.conf \
	file://50-firmware.rules \
	file://firmware.sh \
	"

S = "${@base_conditional('ATHEROS_BUILD_SRC', '1' , '${WORKDIR}/git', '${WORKDIR}/${MACHINE}', d)}"

EXTRA_OEMAKE = "DEL_PLATFORM=${MACHINE} KLIB_BUILD=${STAGING_KERNEL_DIR}"

do_configure_prepend() {
	[ "${ATHEROS_BUILD_SRC}" = "1" ] && MK_SUFFIX=".git" || MK_SUFFIX=".obj"
	cp ${WORKDIR}/Makefile${MK_SUFFIX} ${S}/Makefile
}

do_install_append() {
	install -d ${D}${sysconfdir}/network/if-pre-up.d
	install -m 0755 ${WORKDIR}/atheros ${D}${sysconfdir}/network/if-pre-up.d/
	install -d ${D}${sysconfdir}/modprobe.d
	install -m 0644 ${WORKDIR}/atheros.conf ${D}${sysconfdir}/modprobe.d/
	install -d ${D}${sysconfdir}/udev/rules.d
	install -m 0644 ${WORKDIR}/50-firmware.rules ${D}${sysconfdir}/udev/rules.d/
	install -d ${D}${base_libdir}/udev
	install -m 0755 ${WORKDIR}/firmware.sh ${D}${base_libdir}/udev/
}

FILES_${PN} += " \
	/lib/firmware/ath6k/AR6003/hw2.1.1/athtcmd_ram.bin \
	/lib/firmware/ath6k/AR6003/hw2.1.1/athwlan.bin \
	/lib/firmware/ath6k/AR6003/hw2.1.1/Digi_6203-6233-US.bin \
	/lib/firmware/ath6k/AR6003/hw2.1.1/Digi_6203-6233-World.bin \
	/lib/firmware/ath6k/AR6003/hw2.1.1/fw-4.bin \
	/lib/firmware/ath6k/AR6003/hw2.1.1/nullTestFlow.bin \
	/lib/firmware/ath6k/AR6003/hw2.1.1/utf.bin \
	/lib/udev/firmware.sh \
	"
FILES_${PN}_append_cpx2 = " \
	/lib/firmware/ath6k/AR6003/hw2.1.1/calData_AR6103_Digi_X2e_B.bin \
	/lib/firmware/ath6k/AR6003/hw2.1.1/calData_AR6103_Digi_X2e_B_world.bin \
	"

# Deploy objects tarball if building from sources
do_deploy() {
	if [ "${ATHEROS_BUILD_SRC}" = "1" ]; then
		oe_runmake tarball
		install -d ${DEPLOY_DIR_IMAGE}
		if [ -f "${S}/atheros-${MACHINE}-${SRCREV_SHORT}.tar.gz" ]; then
			cp ${S}/atheros-${MACHINE}-${SRCREV_SHORT}.tar.gz ${DEPLOY_DIR_IMAGE}/
		else
			bberror "Objects tarball not found: ${S}/atheros-${MACHINE}-${SRCREV_SHORT}.tar.gz"
			exit 1
		fi
	fi
}

addtask deploy before do_build after do_install

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(ccardimx28js|cpx2)"

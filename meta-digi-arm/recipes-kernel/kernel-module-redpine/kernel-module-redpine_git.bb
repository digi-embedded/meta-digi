# Copyright (C) 2013 Digi International.

SUMMARY = "Redpine's wireless driver"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://RS.GENR.LNX.SD_GPL/include/ganges_faf.h;endline=6;md5=2b5a9aab5291bd86a1103ca1165f9afa"

inherit module

PR = "${DISTRO}.r0"

# Uncomment to build the driver from sources (internal use only)
# REDPINE_BUILD_SRC ?= "1"

SRCREV = "576a38c9ffca144f4d21bb25f0487045682c956c"

# Checksums for 'redpine-${MACHINE}-${SRCREV_SHORT}.tar.gz' tarballs
TARBALL_MD5_ccimx51js    = "2b0c63a70ba2726baf7408040f0dbc17"
TARBALL_SHA256_ccimx51js = "b689a9c7fba88db06709a84d5232e65234f53f7b43c036cec823d05aa14554e6"
TARBALL_MD5_ccimx53js    = "b124056990c471961ebfeb2cc7c63f87"
TARBALL_SHA256_ccimx53js = "13a2350d9866d1c26aaea9a36da80231602cc49e176baf2191408a685436ad60"

SRC_URI_git = " \
	${DIGI_LOG_GIT}linux-modules/redpine.git;protocol=git \
	"
SRCREV_SHORT = "${@'${SRCREV}'[:7]}"
SRC_URI_obj = " \
	${DIGI_MIRROR}/redpine-${MACHINE}-${SRCREV_SHORT}.tar.gz;md5sum=${TARBALL_MD5};sha256sum=${TARBALL_SHA256} \
	"

SRC_URI  = "${@base_conditional('REDPINE_BUILD_SRC', '1' , '${SRC_URI_git}', '${SRC_URI_obj}', d)}"
SRC_URI += " \
	file://Makefile \
	file://redpine \
	"

S = "${@base_conditional('REDPINE_BUILD_SRC', '1' , '${WORKDIR}/git', '${WORKDIR}/${MACHINE}', d)}"

EXTRA_OEMAKE = "DEL_PLATFORM=${MACHINE}"

do_configure_prepend() {
	cp ${WORKDIR}/Makefile ${S}/
}

do_install_append() {
	install -d ${D}${sysconfdir}/network/if-pre-up.d
	install -m 0755 ${WORKDIR}/redpine ${D}${sysconfdir}/network/if-pre-up.d/
}

FILES_${PN} += "/lib/firmware/redpine/tadm \
		/lib/firmware/redpine/taim \
		/lib/firmware/redpine/instructionSet"

# Deploy objects tarball if building from sources
do_deploy() {
	if [ "${REDPINE_BUILD_SRC}" = "1" ]; then
		oe_runmake tarball
		install -d ${DEPLOY_DIR_IMAGE}
		if [ -f "${S}/redpine-${MACHINE}-${SRCREV_SHORT}.tar.gz" ]; then
			cp ${S}/redpine-${MACHINE}-${SRCREV_SHORT}.tar.gz ${DEPLOY_DIR_IMAGE}/
		else
			bberror "Objects tarball not found: ${S}/redpine-${MACHINE}-${SRCREV_SHORT}.tar.gz"
			exit 1
		fi
	fi
}

addtask deploy before do_build after do_install

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(ccimx51js|ccimx53js)"

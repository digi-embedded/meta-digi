# Copyright (C) 2013 Digi International.

SUMMARY = "Redpine's wireless driver"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://RS.GENR.LNX.SD_GPL/include/ganges_faf.h;endline=6;md5=2b5a9aab5291bd86a1103ca1165f9afa"

inherit module

PR = "${DISTRO}.r0"

SRCREV = "b43b8f5e2d51b24bcc0bc167380cfd07baac81f0"
SRCREV_SHORT = "${@'${SRCREV}'[:7]}"

# Checksums for 'redpine-${MACHINE}-${SRCREV_SHORT}.tar.gz' tarballs
TARBALL_MD5_ccimx51js    = "1a5d7d7b0a41c5dc4e8b9ea44e731264"
TARBALL_SHA256_ccimx51js = "3f855614573da0bc250cfc021f69a1aaba1d7c7c3a6347488604785662d79124"
TARBALL_MD5_ccimx53js    = "4a84d4da7479a20db5ee76f81c33f7b1"
TARBALL_SHA256_ccimx53js = "6e8d35f735172621b5b6c40aafd754aecd8371c6cc1589f9502c8f3098b3a90a"

SRC_URI_git = "${DIGI_LOG_GIT}linux-modules/redpine.git;protocol=git"
SRC_URI_obj = "${DIGI_MIRROR}/redpine-${MACHINE}-${SRCREV_SHORT}.tar.gz;md5sum=${TARBALL_MD5};sha256sum=${TARBALL_SHA256}"
SRC_URI  = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRC_URI_git}', '${SRC_URI_obj}', d)}"
SRC_URI += " \
    file://Makefile \
    file://redpine \
"

S = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${WORKDIR}/git', '${WORKDIR}/${MACHINE}', d)}"

EXTRA_OEMAKE = "DEL_PLATFORM=${MACHINE}"

do_configure_prepend() {
	cp ${WORKDIR}/Makefile ${S}/
}

do_install_append() {
	install -d ${D}${sysconfdir}/network/if-pre-up.d
	install -m 0755 ${WORKDIR}/redpine ${D}${sysconfdir}/network/if-pre-up.d/
}

# Deploy objects tarball if building from sources
do_deploy() {
	if [ "${DIGI_INTERNAL_GIT}" = "1" ]; then
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

FILES_${PN} += " \
    ${base_libdir}/firmware/ \
    ${sysconfdir}/network/ \
"

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(mx5)"

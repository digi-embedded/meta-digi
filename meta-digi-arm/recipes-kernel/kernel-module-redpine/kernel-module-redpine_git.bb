DESCRIPTION = "Redpine's wireless driver"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://RS.GENR.LNX.SD_GPL/include/ganges_faf.h;endline=6;md5=2b5a9aab5291bd86a1103ca1165f9afa"

inherit module

PR = "r0"

# Uncomment to build the driver from sources (internal use only)
# REDPINE_BUILD_SRC ?= "1"

SRCREV = "e43f35983fbcc4d2c537cfb5c0133c7d3cf4fc61"

# Checksums for 'redpine-${MACHINE}-${SRCREV_SHORT}.tar.gz' tarballs
TARBALL_MD5_ccimx51js    = "8ffb32adef6374b535941e31217876ab"
TARBALL_SHA256_ccimx51js = "ba507b694f4dbc98c8ac5dd7b4fd47de6c8f122cd6c4fd3a5cba24ace2c8c7a0"
TARBALL_MD5_ccimx53js    = "c65caf5097892b77024116b76e0c7e16"
TARBALL_SHA256_ccimx53js = "618889737ce0a06aa9bcd8e9c3e1588a2ba4eea8be9d5952d97948dfe2d39c6c"

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

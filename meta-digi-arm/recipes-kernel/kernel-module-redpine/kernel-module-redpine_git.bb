DESCRIPTION = "Redpine's wireless driver"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://RS.GENR.LNX.SD_GPL/include/ganges_faf.h;endline=6;md5=2b5a9aab5291bd86a1103ca1165f9afa"

inherit module

PR = "r0"

REDPINE_BUILD_SRC ?= "1"

SRCREV = "${AUTOREV}"
SRC_URI_git = "${DIGI_LOG_GIT}linux-modules/redpine.git;protocol=git;branch=refs/heads/master"
SRC_URI_obj = "file://redpine-${MACHINE}.tar.gz"

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
		cp ${S}/redpine-${MACHINE}.tar.gz ${DEPLOY_DIR_IMAGE}/
	fi
}

addtask deploy before do_build after do_install

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(ccimx51js|ccimx53js)"

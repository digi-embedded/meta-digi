DESCRIPTION = "Redpine's wireless driver"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://RS.GENR.LNX.SD_NON_GPL/include/ganges_faf.h;endline=6;md5=2b5a9aab5291bd86a1103ca1165f9afa"

inherit module

PR = "r0"

SRCREV = "${AUTOREV}"
SRC_URI = "${DIGI_LOG_GIT}linux-modules/redpine.git;protocol=git;branch=refs/heads/master \
	  file://Makefile \
	  file://redpine"

S = "${WORKDIR}/git"

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

# Create objects tarball and copy to deploy directory
do_deploy() {
	oe_runmake tarball
	install -d ${DEPLOY_DIR_IMAGE}
	cp ${S}/redpine-${MACHINE}.tar.gz ${DEPLOY_DIR_IMAGE}/
}

addtask deploy before do_build after do_install

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(ccimx51js|ccimx53js)"

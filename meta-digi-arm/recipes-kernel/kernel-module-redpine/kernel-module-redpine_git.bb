DESCRIPTION = "Redpine's wireless driver"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://RS.GENR.LNX.SD_NON_GPL/include/ganges_faf.h;endline=6;md5=2b5a9aab5291bd86a1103ca1165f9afa"

inherit module

PR = "r0"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
SRCREV = "${AUTOREV}"
SRC_URI = "${DIGI_LOG_GIT}linux-modules/redpine.git;protocol=git;branch=refs/heads/master \
	  file://0001-redpine-allow-to-build-with-gcc-4.7.patch \
	  file://redpine"

S = "${WORKDIR}/git"

EXTRA_OEMAKE = "-C ${STAGING_KERNEL_DIR}"
EXTRAMAKEFLAGS = "M=${S} CONFIG_DEL_KMOD_REDPINE=y"

module_do_compile() {
        unset CFLAGS CPPFLAGS CXXFLAGS LDFLAGS
        oe_runmake KERNEL_PATH=${STAGING_KERNEL_DIR}   \
                   KERNEL_SRC=${STAGING_KERNEL_DIR}    \
                   KERNEL_VERSION=${KERNEL_VERSION}    \
                   CC="${KERNEL_CC}" LD="${KERNEL_LD}" \
                   AR="${KERNEL_AR}" \
		   ${EXTRAMAKEFLAGS} \
                   ${MAKE_TARGETS}
}

module_do_install() {
        unset CFLAGS CPPFLAGS CXXFLAGS LDFLAGS
        oe_runmake DEPMOD=echo INSTALL_MOD_PATH="${D}" \
                   KERNEL_SRC=${STAGING_KERNEL_DIR} \
                   CC="${KERNEL_CC}" LD="${KERNEL_LD}" \
		   ${EXTRAMAKEFLAGS} \
                   modules_install
}

do_install_append() {
	install -d ${D}${sysconfdir}/network/if-pre-up.d
	install -m 0755 ${WORKDIR}/redpine  ${D}${sysconfdir}/network/if-pre-up.d/
	install -d ${D}/lib/firmware/redpine
	install -m 0755 ${S}/RS.GENR.LNX.SD_GPL/OSD/LINUX/release/tadm ${D}/lib/firmware/redpine/
	install -m 0755 ${S}/RS.GENR.LNX.SD_GPL/OSD/LINUX/release/taim ${D}/lib/firmware/redpine/
	install -m 0755 ${S}/RS.GENR.LNX.SD_GPL/OSD/LINUX/release/instructionSet ${D}/lib/firmware/redpine/
}

FILES_${PN} += " /lib/firmware/redpine/tadm \
		/lib/firmware/redpine/taim \
		/lib/firmware/redpine/instructionSet "

COMPATIBLE_MACHINE = "(ccimx51js|ccimx53js)"

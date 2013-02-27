DESCRIPTION = "Redpine's wireless driver"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://RS.GENR.LNX.SD_NON_GPL/include/ganges_faf.h;endline=6;md5=2b5a9aab5291bd86a1103ca1165f9afa"

inherit module

PR = "r0"

SRCREV = "${AUTOREV}"
SRC_URI = "${DIGI_LOG_GIT}linux-modules/redpine.git;protocol=git;branch=refs/heads/master \
	  file://0001-redpine-allow-to-build-with-gcc-4.7.patch \
	  file://Makefile \
	  file://redpine"

S = "${WORKDIR}/git"

do_configure_prepend() {
	cp ${WORKDIR}/Makefile ${S}/
}

FILES_${PN} += "/lib/firmware/redpine/tadm \
		/lib/firmware/redpine/taim \
		/lib/firmware/redpine/instructionSet"

COMPATIBLE_MACHINE = "(ccimx51js|ccimx53js)"

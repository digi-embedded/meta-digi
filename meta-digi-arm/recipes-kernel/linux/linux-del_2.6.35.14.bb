# Copyright (C) 2012 Digi International

include linux-del.inc

PR = "${INC_PR}.0"

# Uncomment to build the from sources (internal use only)
# KERNEL_BUILD_SRC ?= "1"

SRCREV = "710c80d243367dea31c5236abab3ddae8e93f490"

# Checksums for 'linux-2.6-${SRCREV_SHORT}.tar.gz' tarballs
TARBALL_MD5    = "dedd680314594ab62f3771a1590748c1"
TARBALL_SHA256 = "2e4c35e80bae560c81f6074fb407d07de799582638ae8848190ca502be833869"

LOCALVERSION_mx5 = "mx5"
LOCALVERSION_mxs = "mxs"
LOCALVERSION_cpx2_mxs = "mxs+gateways"

KERNEL_CFG_FRAGS ?= ""
KERNEL_CFG_FRAGS_append_mx5 = " file://config-accel-module.cfg file://config-sahara-module.cfg file://config-camera-module.cfg"
KERNEL_CFG_FRAGS_append_ccimx51js = " file://config-battery-module.cfg"
KERNEL_CFG_FRAGS_append_ccardimx28js = " ${@base_contains('DISTRO_FEATURES', 'x11', 'file://config-fb.cfg file://config-touch.cfg', '', d)}"
KERNEL_CFG_FRAGS_append_ccardimx28js = " ${@base_contains('MACHINE_FEATURES', 'alsa', 'file://config-sound.cfg', '', d)}"

SRCREV_SHORT = "${@'${SRCREV}'[:7]}"
SRC_URI_tarball = " \
        ${DIGI_MIRROR}/linux-2.6-${SRCREV_SHORT}.tar.gz;md5sum=${TARBALL_MD5};sha256sum=${TARBALL_SHA256} \
        "

SRC_URI_git = " \
	${DIGI_LOG_GIT}linux-2.6.git;protocol=git;branch=refs/heads/master \
	"

SRC_URI  = "${@base_conditional('KERNEL_BUILD_SRC', '1' , '${SRC_URI_git}', '${SRC_URI_tarball}', d)}"
SRC_URI += " \
	file://defconfig \
        ${KERNEL_CFG_FRAGS} \
	"

S  = "${@base_conditional('KERNEL_BUILD_SRC', '1' , '${WORKDIR}/git', '${WORKDIR}/linux-2.6-${SRCREV_SHORT}', d)}"
SCMVERSION = "${@base_conditional('KERNEL_BUILD_SRC', '1' , '1', '0', d)}"

FILES_kernel-image += "/boot/config*"

COMPATIBLE_MACHINE = "(mxs|mx5)"

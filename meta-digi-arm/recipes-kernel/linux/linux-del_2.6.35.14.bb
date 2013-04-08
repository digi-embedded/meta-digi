# Copyright (C) 2012 Digi International

include linux-del.inc

PR = "${DISTRO}.${INC_PR}.0"

# Uncomment to build the from sources (internal use only)
# KERNEL_BUILD_SRC ?= "1"

SRCREV = "9fd8c01cf3453b8e9dd668ecfd0a26a3253c33a1"

# Checksums for 'linux-2.6-${SRCREV_SHORT}.tar.gz' tarballs
TARBALL_MD5    = "b18c09c4c5624d5d4f2644412810e61b"
TARBALL_SHA256 = "0e14d1c27799ade5f404a101c9906dde35ecd405b6ec09a876255f9ca5609eb5"

SRCREV_SHORT = "${@'${SRCREV}'[:7]}"

LOCALVERSION_mx5 = "mx5+${SRCREV_SHORT}"
LOCALVERSION_mxs = "mxs+${SRCREV_SHORT}"
LOCALVERSION_cpx2_mxs = "mxs+gateways+${SRCREV_SHORT}"

KERNEL_CFG_FRAGS ?= ""
KERNEL_CFG_FRAGS_append_mx5 = " file://config-accel-module.cfg file://config-sahara-module.cfg file://config-camera-module.cfg"
KERNEL_CFG_FRAGS_append_ccimx51js = " file://config-battery-module.cfg"
KERNEL_CFG_FRAGS_append_ccardimx28js = " ${@base_contains('DISTRO_FEATURES', 'x11', 'file://config-fb.cfg file://config-touch.cfg', '', d)}"
KERNEL_CFG_FRAGS_append_ccardimx28js = " ${@base_contains('MACHINE_FEATURES', 'alsa', 'file://config-sound.cfg', '', d)}"

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

# Copyright (C) 2012 Digi International

include linux-del.inc

PR = "${DISTRO}.${INC_PR}.0"

# Uncomment to build the from sources (internal use only)
# KERNEL_BUILD_SRC ?= "1"

SRCREV = "d615e9350f4d14e8a0177ff4a3da8639046657eb"

# Checksums for 'linux-2.6-${SRCREV_SHORT}.tar.gz' tarballs
TARBALL_MD5    = "7cdc5187de9b416cfd4a619cd4c83742"
TARBALL_SHA256 = "5071dfc96c7340ca4e4b3f9c54771e146f3a4b46cb30c3c92c33c7a4d8ead7de"

SRCREV_SHORT = "${@'${SRCREV}'[:7]}"

LOCALVERSION_mx5 = "mx5+${SRCREV_SHORT}"
LOCALVERSION_mxs = "mxs+${SRCREV_SHORT}"
LOCALVERSION_cpx2_mxs = "mxs+gateways+${SRCREV_SHORT}"

KERNEL_CFG_FRAGS ?= ""
KERNEL_CFG_FRAGS_append_mx5 = "file://config-sahara-module.cfg file://config-camera-module.cfg"
KERNEL_CFG_FRAGS_append_mx5 = " ${@base_contains('MACHINE_FEATURES', 'accelerometer', 'file://config-accel-module.cfg', '', d)}"
KERNEL_CFG_FRAGS_append_mx5 = " ${@base_contains('MACHINE_FEATURES', 'ext-eth', 'file://config-ext-eth-module.cfg', '', d)}"
KERNEL_CFG_FRAGS_append_mx5 = " ${@base_contains('MACHINE_FEATURES', 'wifi', 'file://config-wireless-redpine.cfg', '', d)}"
KERNEL_CFG_FRAGS_append_ccimx51js = " file://config-battery-module.cfg"
KERNEL_CFG_FRAGS_append_mxs = " ${@base_contains('MACHINE_FEATURES', 'wifi', 'file://config-wireless-atheros.cfg', '', d)}"
KERNEL_CFG_FRAGS_append_mxs = " ${@base_contains('MACHINE_FEATURES', 'bluetooth', 'file://config-bluetooth-atheros.cfg', '', d)}"
KERNEL_CFG_FRAGS_append_ccardimx28js = " ${@base_contains('MACHINE_FEATURES', '1-wire', 'file://config-1-wire.cfg', '', d)}"
KERNEL_CFG_FRAGS_append_ccardimx28js = " ${@base_contains('MACHINE_FEATURES', 'ext-eth', 'file://config-ext-eth.cfg', '', d)}"
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

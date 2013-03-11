# Copyright (C) 2012 Digi International

include linux-del.inc

PR = "${INC_PR}.0"

SRCREV = "${AUTOREV}"

LOCALVERSION_mx5 = "mx5+master"
LOCALVERSION_mxs = "mxs+master"
LOCALVERSION_cpx2_mxs = "mxs+gateways_master"

KERNEL_CFG_FRAGS ?= ""
KERNEL_CFG_FRAGS_append_mx5 = " file://config-accel-module.cfg file://config-sahara-module.cfg file://config-camera-module.cfg"
KERNEL_CFG_FRAGS_append_ccimx51js = " file://config-battery-module.cfg"
KERNEL_CFG_FRAGS_append_ccardimx28js = " ${@base_contains('DISTRO_FEATURES', 'x11', 'file://config-fb.cfg file://config-touch.cfg', '', d)}"
KERNEL_CFG_FRAGS_append_ccardimx28js = " ${@base_contains('MACHINE_FEATURES', 'alsa', 'file://config-sound.cfg', '', d)}"

SRC_URI = " \
	${DIGI_LOG_GIT}linux-2.6.git;protocol=git;branch=refs/heads/master \
	file://defconfig \
	${KERNEL_CFG_FRAGS} \
	"

SRC_URI_cpx2 = " \
	${DIGI_LOG_GIT}linux-2.6.git;protocol=git;branch=refs/heads/gateways/master \
	file://defconfig \
	"

FILES_kernel-image += "/boot/config*"

COMPATIBLE_MACHINE = "(mxs|mx5)"

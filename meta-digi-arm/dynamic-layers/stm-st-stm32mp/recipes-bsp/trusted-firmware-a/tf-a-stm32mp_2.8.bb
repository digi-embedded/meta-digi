#
# Copyright (C) 2024 Digi International Inc.
#
require tf-a-stm32mp2-common.inc
require tf-a-stm32mp2.inc

SUMMARY = "Trusted Firmware-A for STM32MP1"
LICENSE = "BSD-3-Clause"

# Select internal or Github TF-A repo
TFA_URI_STASH = "${DIGI_MTK_GIT}/emp/arm-trusted-firmware.git;protocol=ssh"
TFA_URI_GITHUB = "${DIGI_GITHUB_GIT}/arm-trusted-firmware.git;protocol=https"
TFA_GIT_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1' , '${TFA_URI_STASH}', '${TFA_URI_GITHUB}', d)}"

SRCBRANCH = "v2.8/stm32mp/maint_ccmp2-cc91-beta"
SRCREV = "${AUTOREV}"

SRC_URI = " \
    ${TFA_GIT_URI};branch=${SRCBRANCH} \
"

TF_A_VERSION = "v2.8.12"
TF_A_RELEASE = "beta-r1"

# Configure settings
TFA_PLATFORM  = "stm32mp1"
TFA_ARM_MAJOR = "7"
TFA_ARM_ARCH  = "aarch32"

TFA_PLATFORM:aarch64  = "stm32mp2"
TFA_ARM_MAJOR:aarch64 = "8"
TFA_ARM_ARCH:aarch64  = "aarch64"

# Enable the wrapper for debug
TF_A_ENABLE_DEBUG_WRAPPER ?= "1"

# ---------------------------------
# Configure archiver use
# ---------------------------------
include ${@oe.utils.ifelse(d.getVar('ST_ARCHIVER_ENABLE') == '1', 'tf-a-stm32mp-archiver.inc','')}

COMPATIBLE_MACHINE = "(ccmp2)"

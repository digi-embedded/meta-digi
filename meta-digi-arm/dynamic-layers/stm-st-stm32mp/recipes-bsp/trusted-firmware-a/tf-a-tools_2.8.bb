require tf-a-stm32mp2-common.inc
require tf-a-tools.inc

SUMMARY = "Cert_create & Fiptool for fip generation for Trusted Firmware-A"
LICENSE = "BSD-3-Clause"

# Select internal or Github TF-A repo
TFA_URI_STASH = "${DIGI_MTK_GIT}/emp/arm-trusted-firmware.git;protocol=ssh"
TFA_URI_GITHUB = "${DIGI_GITHUB_GIT}/arm-trusted-firmware.git;protocol=https"
TFA_GIT_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1' , '${TFA_URI_STASH}', '${TFA_URI_GITHUB}', d)}"

SRCBRANCH = "v2.8/stm32mp/maint_ccmp2-beta"
SRCREV = "779078679e4714addd14e58efb2564e050a0f016"

SRC_URI = " \
    ${TFA_GIT_URI};nobranch=1 \
"

# Configure settings
TFA_PLATFORM = "stm32mp1"
TFA_PLATFORM:class-native  = "stm32mp2"
TFA_PLATFORM:class-nativesdk  = "stm32mp2"

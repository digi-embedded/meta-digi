# Copyright (C) 2016-2020 Digi International.

# Default TrustFence SDK tools
TRUSTFENCE_SDK_TOOLS ?= "\
    nativesdk-trustfence-sign-tools \
    nativesdk-trustfence-cst \
"

RDEPENDS:${PN} += " \
    ${TRUSTFENCE_SDK_TOOLS} \
    ${@oe.utils.conditional('TRUSTFENCE_SIGN_MODE', 'AHAB', 'nativesdk-imx-mkimage', '', d)} \
"

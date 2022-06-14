# Copyright (C) 2016-2020 Digi International.

# Default TrustFence SDK tools
TRUSTFENCE_SDK_TOOLS ?= "\
    nativesdk-trustfence-sign-tools \
    nativesdk-trustfence-cst \
"

RDEPENDS_${PN} += " \
    ${@oe.utils.conditional('TRUSTFENCE_SIGN_MODE', 'AHAB', '${TRUSTFENCE_SDK_TOOLS} nativesdk-imx-mkimage', '${TRUSTFENCE_SDK_TOOLS}', d)} \
"

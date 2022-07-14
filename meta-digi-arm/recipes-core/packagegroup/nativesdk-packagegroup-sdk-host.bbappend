# Copyright (C) 2016-2022 Digi International.

IMX_TRUSTFENCE_SDK_TOOLS ?= " \
    nativesdk-trustfence-sign-tools \
    nativesdk-trustfence-cst \
    ${@oe.utils.conditional('TRUSTFENCE_SIGN_MODE', 'AHAB', 'nativesdk-imx-mkimage', '', d)} \
"

RDEPENDS:${PN} += " \
    ${@oe.utils.conditional('DEY_BUILD_PLATFORM', 'NXP', '${IMX_TRUSTFENCE_SDK_TOOLS}', '', d)} \
"

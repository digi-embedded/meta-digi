# Copyright (C) 2016-2023 Digi International.

IMX_OPTEE_SDK_RDEPENDS ?= " \
    nativesdk-python3-cryptography \
    nativesdk-python3-pyelftools \
"

IMX_TRUSTFENCE_SDK_TOOLS ?= " \
    nativesdk-trustfence-sign-tools \
    nativesdk-trustfence-cst \
    ${@oe.utils.conditional('TRUSTFENCE_SIGN_MODE', 'AHAB', 'nativesdk-imx-mkimage', '', d)} \
"

RDEPENDS:${PN} += " \
    ${@oe.utils.conditional('DEY_SOC_VENDOR', 'NXP', '${IMX_OPTEE_SDK_RDEPENDS} ${IMX_TRUSTFENCE_SDK_TOOLS}', '', d)} \
"

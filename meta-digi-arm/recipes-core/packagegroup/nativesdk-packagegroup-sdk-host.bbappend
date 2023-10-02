# Copyright (C) 2016-2023 Digi International.

IMX_OPTEE_SDK_RDEPENDS ?= " \
    nativesdk-python3-cryptography \
    nativesdk-python3-pyelftools \
"

IMX_TRUSTFENCE_SDK_TOOLS ?= " \
    nativesdk-trustfence-cst \
    ${@oe.utils.conditional('TRUSTFENCE_SIGN_MODE', 'AHAB', 'nativesdk-imx-mkimage', '', d)} \
"

STM_TRUSTFENCE_SDK_TOOLS ?= " \
    nativesdk-trustfence-stm-signtools \
"

RDEPENDS:${PN} += " \
    nativesdk-trustfence-sign-tools \
    ${@oe.utils.conditional('DEY_SOC_VENDOR', 'NXP', '${IMX_OPTEE_SDK_RDEPENDS} ${IMX_TRUSTFENCE_SDK_TOOLS}', '', d)} \
    ${@oe.utils.conditional('DEY_SOC_VENDOR', 'STM', '${STM_TRUSTFENCE_SDK_TOOLS}', '', d)} \
"

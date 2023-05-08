# Copyright (C) 2023, Digi International Inc.

require trustfence-sign-tools.inc
inherit nativesdk

RDEPENDS:${PN} = " \
    ${@oe.utils.conditional('DEY_SOC_VENDOR', 'NXP', 'nativesdk-trustfence-cst', '', d)} \
    ${@oe.utils.conditional('DEY_SOC_VENDOR', 'STM', 'nativesdk-trustfence-stm-signtools', '', d)} \
"

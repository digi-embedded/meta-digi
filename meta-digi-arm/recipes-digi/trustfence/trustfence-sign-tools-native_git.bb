# Copyright (C) 2023, Digi International Inc.

require trustfence-sign-tools.inc
inherit native

RDEPENDS:${PN} = " \
    ${@oe.utils.conditional('DEY_SOC_VENDOR', 'NXP', 'trustfence-cst-native', '', d)} \
    ${@oe.utils.conditional('DEY_SOC_VENDOR', 'STM', 'trustfence-stm-signtools-native', '', d)} \
    coreutils-native \
    util-linux-native \
    ${@oe.utils.conditional('TRUSTFENCE_SIGN_MODE', 'AHAB', 'imx-mkimage-native', '', d)} \
"

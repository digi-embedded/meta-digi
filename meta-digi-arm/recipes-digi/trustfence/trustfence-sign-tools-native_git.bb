# Copyright (C) 2023, Digi International Inc.

require trustfence-sign-tools.inc
inherit native

RDEPENDS:${PN} = "trustfence-cst-native coreutils-native util-linux-native"
RDEPENDS:${PN} += "${@oe.utils.conditional('TRUSTFENCE_SIGN_MODE', 'AHAB', 'imx-mkimage-native', '', d)}"

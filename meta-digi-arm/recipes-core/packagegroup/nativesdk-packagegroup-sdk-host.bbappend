# Copyright (C) 2016 Digi International.

RDEPENDS_${PN} += " \
    ${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', 'nativesdk-trustfence-sign-tools nativesdk-trustfence-cst', '', d)} \
"

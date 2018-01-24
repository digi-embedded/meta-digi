# Copyright (C) 2016 Digi International.

RDEPENDS_${PN} += " \
    ${@base_conditional('TRUSTFENCE_SIGN', '1', 'nativesdk-trustfence-sign-tools nativesdk-trustfence-cst', '', d)} \
"

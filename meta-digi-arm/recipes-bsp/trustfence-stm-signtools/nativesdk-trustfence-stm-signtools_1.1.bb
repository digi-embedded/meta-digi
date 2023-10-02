# Copyright (C) 2023 Digi International.

require trustfence-stm-signtools.inc
inherit nativesdk

# STM signing tools binaries depend on libQt5Core.so.5
RDEPENDS:${PN} += " \
    nativesdk-qtbase \
"

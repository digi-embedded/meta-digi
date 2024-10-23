# Copyright (C) 2023-2024, Digi International Inc.

# qtdeviceutilities provides a networksettings module that depends
# on "connman". This conflicts with NetworkManager
RDEPENDS:${PN}:remove:dey = "qtdeviceutilities"

# Historically, qtpdf was added to this packagegroup only when "webengine" is
# in DISTRO_FEATURES, but now it always gets added and causes build errors.
# Restore this logic to prevent the errors while respecting the specific
# architecture overrides from the original recipe.
RDEPENDS:${PN}:remove:dey = "qtpdf"
RDEPENDS:${PN}:append:aarch64 = " ${@bb.utils.contains('DISTRO_FEATURES', 'webengine', 'qtpdf', '', d)}"
RDEPENDS:${PN}:append:armv6 = " ${@bb.utils.contains('DISTRO_FEATURES', 'webengine', 'qtpdf', '', d)}"
RDEPENDS:${PN}:append:armv7a = " ${@bb.utils.contains('DISTRO_FEATURES', 'webengine', 'qtpdf', '', d)}"
RDEPENDS:${PN}:append:armv7ve = " ${@bb.utils.contains('DISTRO_FEATURES', 'webengine', 'qtpdf', '', d)}"
RDEPENDS:${PN}:append:x86-64 = " ${@bb.utils.contains('DISTRO_FEATURES', 'webengine', 'qtpdf', '', d)}"

# qtdoc has a hardcoded dependency with qtpdf, so remove it as well
RDEPENDS:${PN}:remove:dey = "qtdoc"

# Copyright (C) 2024, Digi International Inc.

# Remove addons packagegroup in favor of our custom dey one
RDEPENDS:${PN}:remove:dey = "packagegroup-qt6-addons"
RDEPENDS:${PN}:append:dey = " packagegroup-qt6-dey"

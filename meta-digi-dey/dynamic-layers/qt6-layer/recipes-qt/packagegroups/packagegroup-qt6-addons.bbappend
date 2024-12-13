# Copyright (C) 2023-2024, Digi International Inc.

# qtdeviceutilities provides a networksettings module that depends
# on "connman". This conflicts with NetworkManager
RDEPENDS:${PN}:remove:dey = "qtdeviceutilities"

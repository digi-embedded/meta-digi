# Copyright (c) 2023, Digi International Inc.

# qtdeviceutilities provides a networksettings module that depends
# on "connman". This conflicts with NetworkManager
RDEPENDS:${PN}:remove:dey = "qtdeviceutilities"

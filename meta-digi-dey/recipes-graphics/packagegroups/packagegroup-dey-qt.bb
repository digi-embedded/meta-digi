#
# Copyright (C) 2013-2023, Digi International Inc.
#
SUMMARY = "QT packagegroup for DEY image"

PACKAGE_ARCH = "${MACHINE_ARCH}"
inherit packagegroup

inherit qt-version

RDEPENDS:${PN} += "${@oe.utils.conditional('QT_VERSION', '', '', 'packagegroup-${QT_VERSION}-dey', d)}"

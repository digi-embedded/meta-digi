#
# Abstract QT related metadata in this class with two purposes:
#
#   - Remove the mandatory dependence of DEY in meta-qtX
#   - Support multiple QT versions
#
# Copyright (c) 2023, Digi International Inc.
#

QT_AVAILABLE = "${@bb.utils.contains_any('BBFILE_COLLECTIONS', 'qt5-layer qt6-layer', 'true', 'false', d)}"
QT_VERSION = \
    "${@bb.utils.contains('BBFILE_COLLECTIONS', 'qt6-layer', 'qt6', \
        bb.utils.contains('BBFILE_COLLECTIONS', 'qt5-layer', 'qt5', '', d), d)}"
QT_POPULATE_SDK = "${@oe.utils.vartrue('QT_AVAILABLE', 'populate_sdk_${QT_VERSION}', '', d)}"

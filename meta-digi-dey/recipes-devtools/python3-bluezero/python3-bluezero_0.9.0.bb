# Copyright (C) 2022-2025, Digi International Inc.

SUMMARY = "Library providing an API to access to Bluez with zero boilerplate code"
DESCRIPTION = "The library will use calls to the BlueZ D-Bus API and use ‘sensible’ defaults to help with that simplification."
HOMEPAGE = "https://github.com/ukBaz/python-bluezero"
SECTION = "devel/python"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=66f12994d9f609ef52171aaa0bd371a9"

SRC_URI[sha256sum] = "2512935e094e3afd21ca9d4cb1b9aaa88a524e9538ae81305b2086f09f0eee17"

inherit setuptools3 pypi

RDEPENDS:${PN} += " \
    python3-dbus \
    python3-pygobject \
"

RPROVIDES:${PN} = "python3-bluezero"

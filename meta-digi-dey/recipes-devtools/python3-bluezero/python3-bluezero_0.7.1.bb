# Copyright (C) 2022 Digi International Inc.

SUMMARY = "Library providing an API to access to Bluez with zero boilerplate code"
DESCRIPTION = "The library will use calls to the BlueZ D-Bus API and use ‘sensible’ defaults to help with that simplification."
HOMEPAGE = "https://github.com/ukBaz/python-bluezero"
SECTION = "devel/python"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=66f12994d9f609ef52171aaa0bd371a9"

SRC_URI[sha256sum] = "f146feb65ee9f6fd9f3638ff0a44df9fd6efb48cf66a39ce51a62a7d38ab5206"

inherit setuptools3 pypi

RDEPENDS:${PN} += " \
    python3-dbus \
    python3-pygobject \
"

RPROVIDES:${PN} = "python3-bluezero"

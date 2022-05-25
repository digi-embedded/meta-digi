# Copyright (C) 2019 Digi International Inc.

SUMMARY = "Python library to interact with Digi International's XBee radio frequency modules."
HOMEPAGE = "https://github.com/digidotcom/python-xbee"

LICENSE = "MPL-2.0"

PYPI_PACKAGE = "digi-xbee"

RDEPENDS:${PN} = "python3-pyserial"

inherit pypi setuptools3

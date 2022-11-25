# Copyright (C) 2022 Digi International Inc.

SUMMARY = "Python library to interact with Digi International's XBee radio frequency modules."
DESCRIPTION = "The XBee Python Library provides the ability to communicate with remote nodes in the network, IoT devices and other interfaces of the local device."
HOMEPAGE = "https://github.com/digidotcom/python-xbee"
SECTION = "devel/python"
LICENSE = "MPL-2.0"
LIC_FILES_CHKSUM = "file://LICENSE.txt;md5=f74526e982749d58a537b3fcdb5d3318"

SRC_URI[md5sum] = "0608ecc8051b31d821b58dcec5396705"
SRC_URI[sha256sum] = "3b10e749431f406d80c189d872f4673b8d3cd510f7b411f817780a0e72499cd2"

PYPI_PACKAGE = "digi-xbee"

inherit pypi setuptools3

RDEPENDS:${PN} = "python3-asyncio python3-pyserial"

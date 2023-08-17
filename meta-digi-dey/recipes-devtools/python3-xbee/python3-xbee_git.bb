# Copyright (C) 2022, 2023 Digi International Inc.

SUMMARY = "Python library to interact with Digi International's XBee radio frequency modules."
DESCRIPTION = "The XBee Python Library provides the ability to communicate with remote nodes in the network, IoT devices and other interfaces of the local device."
HOMEPAGE = "https://github.com/digidotcom/python-xbee"
SECTION = "devel/python"
LICENSE = "MPL-2.0"
LIC_FILES_CHKSUM = "file://LICENSE.txt;md5=f74526e982749d58a537b3fcdb5d3318"

SRCBRANCH ?= "master"
SRCREV = "1024d8954e1b445fe6aacd7f0880de65978c351b"

SRC_URI = " \
    git://github.com/digidotcom/xbee-python.git;protocol=https;branch=${SRCBRANCH} \
"

S = "${WORKDIR}/git"

inherit setuptools3

RDEPENDS:${PN} = "python3-asyncio python3-pyserial"

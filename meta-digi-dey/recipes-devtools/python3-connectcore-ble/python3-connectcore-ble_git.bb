# Copyright (C) 2022-2024, Digi International Inc.

SUMMARY = "Bluetooth Low Energy Python library for ConnetCore devices"
DESCRIPTION = "The ConnectCore BLE Python library allows your Digi International's ConnectCore modules to interact with mobile applications."
HOMEPAGE = "https://github.com/digi-embedded/connectcore-ble-python"
SECTION = "devel/python"
LICENSE = "MPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MPL-2.0;md5=815ca599c9df247a0c7f619bab123dad"

SRCBRANCH ?= "master"
SRCREV = "70245c4f4de7f2ffae899fd7cf267d9ad6db7ae0"
PV = "1.0.7"

SRC_URI = " \
    ${DIGI_GITHUB_GIT}/connectcore-ble-python.git;protocol=https;branch=${SRCBRANCH} \
"

S = "${WORKDIR}/git"

inherit setuptools3

RDEPENDS:${PN} += " \
    python3-core \
    python3-bluezero \
    python3-pycryptodome \
    python3-srp \
    python3-xbee \
"

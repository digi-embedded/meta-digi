# Copyright (C) 2022, Digi International Inc.

SUMMARY = "Bluetooth Low Energy Python library for ConnetCore devices"
DESCRIPTION = "The ConnectCore BLE Python library allows your Digi International's ConnectCore modules to interact with mobile applications."
HOMEPAGE = "https://github.com/digi-embedded/connectcore-ble-python"
SECTION = "devel/python"
LICENSE = "MPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MPL-2.0;md5=815ca599c9df247a0c7f619bab123dad"

SRCBRANCH ?= "master"
SRCREV = "23e8bf3dc438edae117fb1d721c06ec9b626dcec"

CONNECTCORE_BLE_URI_STASH = "${DIGI_MTK_GIT}/python/connectcore-ble-python.git;protocol=ssh"
CONNECTCORE_BLE_URI_GITHUB = "${DIGI_GITHUB_GIT}/connectcore-ble-python.git;protocol=https"
CONNECTCORE_BLE_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1' , '${CONNECTCORE_BLE_URI_STASH}', '${CONNECTCORE_BLE_URI_GITHUB}', d)}"

SRC_URI = " \
    ${CONNECTCORE_BLE_URI};nobranch=1 \
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

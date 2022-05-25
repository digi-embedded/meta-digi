# Copyright (C) 2014 Freescale Semiconductor
# Copyright 2017-2018 NXP
# Released under the MIT license (see COPYING.MIT for the terms)

DESCRIPTION = "Package group for Qt5 demos"
LICENSE = "MIT"

inherit packagegroup

RDEPENDS:${PN}:append = " \
    qtserialport \
"

# Install the following apps on SoC with GPU
RDEPENDS:${PN}:append:imxgpu = " \
    qtbase-examples \
    qtdeclarative-examples \
    quitindicators \
    qt5-demo-extrafiles \
    qt5ledscreen \
    quitbattery \
    qt5everywheredemo \
    qt5nmapcarousedemo \
    qt5nmapper \
    cinematicexperience-rhi \
"

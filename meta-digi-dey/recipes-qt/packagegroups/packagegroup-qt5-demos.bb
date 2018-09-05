# Copyright (C) 2014 Freescale Semiconductor
# Copyright 2017-2018 NXP
# Released under the MIT license (see COPYING.MIT for the terms)

DESCRIPTION = "Package group for Qt5 demos"
LICENSE = "MIT"

inherit packagegroup

RDEPENDS_${PN}_append = " \
    qtserialport \
"

# Install the following apps on SoC with GPU
RDEPENDS_${PN}_append_imxgpu = " \
    cinematicexperience \
    qtbase-examples \
    qtconnectivity-examples \
    qtdeclarative-examples \
    qtmultimedia-examples \
    qtsvg-examples \
    qt5-demo-extrafiles \
    qt5everywheredemo \
"

RDEPENDS_${PN}_append_imxgpu3d = " \
    qt3d-examples \
"

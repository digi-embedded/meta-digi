# Copyright (C) 2020 Digi International

# Use the sources in poky's sumo recipe
FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"
LIC_FILES_CHKSUM = "file://LICENSE.md;md5=dcf473723faabf17baa9b5f2207599d0 \
                    file://triangle/triangle.cpp;endline=12;md5=bccd1bf9cadd9e10086cf7872157e4fa"

SRC_URI = "git://github.com/SaschaWillems/Vulkan.git \
           file://0001-Support-installing-demos-support-out-of-tree-builds.patch \
           file://0001-Don-t-build-demos-with-questionably-licensed-data.patch \
           file://0001-Fix-build-on-x86.patch \
"
SRCREV = "18df00c7b4677b0889486e16977857aa987947e2"

DEPENDS_remove = "vulkan"
DEPENDS_append = " vulkan-headers vulkan-loader"

# The vulkan-validationlayers package is necessary for the demos to work
RDEPENDS_${PN} = "vulkan-validationlayers"


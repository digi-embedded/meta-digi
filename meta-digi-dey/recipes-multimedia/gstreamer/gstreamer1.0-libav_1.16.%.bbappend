# Copyright (C) 2020 Digi International Inc.

PV = "1.16.0"

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://gtkdoc-no-tree.patch"

SRC_URI[md5sum] = "e3a201a45985ddc1327cd496046ca818"
SRC_URI[sha256sum] = "dfac119043a9cfdcacd7acde77f674ab172cf2537b5812be52f49e9cddc53d9a"

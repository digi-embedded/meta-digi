# Copyright (C) 2022 Digi International

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI_append = " file://0001-include-ccimx8m_common-increase-rootfs-partition-siz.patch"

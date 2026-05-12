# Copyright (C) 2026, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI:append = " \
    file://0001-create_st_fip_binary-use-verbose-hexdump-for-encrypt.patch;patchdir=${WORKDIR} \
"

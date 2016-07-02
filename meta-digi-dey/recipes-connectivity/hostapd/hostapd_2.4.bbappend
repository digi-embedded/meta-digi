# Copyright (C) 2016 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BP}:"

# The recipe uses a different "$S" directory so point the patch to the hostapd
# tarball directory.
SRC_URI_append_ccimx6ul = " file://fix_num_probereq_cb_clearing.patch;patchdir=.."

PACKAGE_ARCH = "${MACHINE_ARCH}"

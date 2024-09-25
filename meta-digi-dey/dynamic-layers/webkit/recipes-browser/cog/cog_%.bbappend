# Copyright (C) 2020-2023, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI:append = " \
    file://0001-wl-Fix-wrong-wl_shm-for-cursor.patch \
"

EXTRA_OECMAKE += "-DCOG_HOME_URI=http://127.0.0.1/"

# drm PACKAGECONFIG pulls in libgbm dependency, which isn't available
# on the i.MX6 and ccmp1
PACKAGECONFIG:remove:ccimx6 = "drm"
PACKAGECONFIG:remove:ccmp1 = "drm"

# Copyright 2020-2023 Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI:append = " \
    file://0001-wl-Fix-wrong-wl_shm-for-cursor.patch \
"

EXTRA_OECMAKE += "-DCOG_HOME_URI=http://127.0.0.1/"

# Starting in v0.12.X, we need to explicitly enable the wl PACKAGECONFIG to
# include the wayland platform
PACKAGECONFIG += "wl"

# drm PACKAGECONFIG pulls in libgbm dependency, which isn't available
# on the i.MX6
PACKAGECONFIG:remove:ccimx6 = "drm"

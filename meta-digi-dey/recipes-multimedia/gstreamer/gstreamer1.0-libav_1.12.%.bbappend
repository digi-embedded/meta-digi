FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI_IMX_PATCHES = " \
    file://0002-gstavcodecmap-Do-not-require-a-channel-mask.patch \
"

SRC_URI_append_mx6 = "${SRC_URI_IMX_PATCHES}"
SRC_URI_append_mx7 = "${SRC_URI_IMX_PATCHES}"

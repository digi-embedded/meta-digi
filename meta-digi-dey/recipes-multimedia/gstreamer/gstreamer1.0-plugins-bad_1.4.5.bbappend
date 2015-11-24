# Copyright (C) 2015 Digi International

FILESEXTRAPATHS_prepend := "${THISDIR}/${BP}:"

SRC_URI_append_ccimx6 = " \
    file://egl-workaround-for-eglCreateContext-isn-t-thread-safe.patch \
    file://camerabin-Add-one-property-to-set-sink-element-for-video.patch \
    file://0011-videoparse-modifiy-the-videoparse-rank.patch \
    file://camerabin-examples-memory-leak-in-camerabin-examples-01.patch \
    file://camerabin-examples-memory-leak-in-camerabin-examples-02.patch \
    file://dvbsuboverlay-Set-query-ALLOCATION-need_pool-to-FALSE.patch \
    file://0002-mpegtsmux-Need-get-pid-when-create-streams.patch \
    file://0006-h263parse_fix_CPFMT_parsing.patch \
    file://0009-mpeg4videoparse-Need-detect-picture-coding-type-when.patch \
    file://0010-mpegvideoparse-Need-detect-picture-coding-type-when-.patch \
    file://0012-glfilter-Lost-frame-rate-info-when-fixate-caps.patch \
    file://0014-opencv-rename-gstopencv.c-to-gstopencv.cpp.patch \
    file://0015-opencv-Add-video-stitching-support.patch \
    file://0016-PATCH-gstaggregator-memory-leak-increasing-a-lot-aft.patch \
    file://1.4.5-Use-viv-direct-texture-to-bind-buffer.patch \
    file://0001-Support-croping-and-alignment-handling.patch \
    file://Fix-warnnig-log-in-glfilter.patch \
    file://Adding-some-fragment-shaders-for-glshader-plugin.patch \
    file://Fix-for-gl-plugin-not-built-in-wayland-backend.patch \
    file://0003-glimagesink-Add-fps-print-in-glimagesink.patch \
    file://0004-gl-fb-Support-fb-backend-for-gl-plugins.patch \
    file://0005-gl-wayland-Make-it-always-fullscreen-1024x768.patch \
    file://0007-glfilter-Fix-video-is-tearing-after-enab.patch \
    file://0008-gl-Fix-glimagesink-loop-playback-failed-in-wayland.patch \
    file://0017-MMFMWK-6778-Support-more-format-in-direct-viv.patch \
"


# Revert Poky commit cdc2c8aeaa96b07dfc431a4cf0bf51ef7f8802a3 (move EGL to Wayland)
# Otherwise 'glimagesink' for X11 is not compiled and for example this sink is needed
# by 'imxcamera' application (distributed by FSL in binary form)
PACKAGECONFIG[gles2]   = "--enable-gles2 --enable-egl,--disable-gles2 --disable-egl,virtual/libgles2 virtual/egl"
PACKAGECONFIG[wayland] = "--enable-wayland --disable-x11,--disable-wayland,wayland"

# include fragment shaders
FILES_${PN}-opengl += "/usr/share/*.fs"

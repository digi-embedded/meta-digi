# Copyright 2015-2017, Digi International Inc.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BP}:"

SRC_URI_append_imxgpu2d = " \
    file://0001-mpegtsmux-Need-get-pid-when-create-streams.patch \
    file://0002-mpeg4videoparse-Need-detect-picture-coding-type-when.patch \
    file://0003-mpegvideoparse-Need-detect-picture-coding-type-when-.patch \
    file://0004-modifiy-the-videoparse-rank.patch \
    file://0005-glfilter-Lost-frame-rate-info-when-fixate-caps.patch \
    file://0006-opencv-Add-video-stitching-support-based-on-Open-CV.patch \
    file://0007-camerabin-Add-one-property-to-set-sink-element-for-v.patch \
    file://0008-Fix-for-gl-plugin-not-built-in-wayland-backend.patch \
    file://0009-gl-wayland-fix-loop-test-hang-in-glimagesink.patch \
    file://0010-Fix-glimagesink-wayland-resize-showed-blurred-screen.patch \
    file://0011-support-video-crop-for-glimagesink.patch \
    file://0012-Add-fps-print-in-glimagesink.patch \
    file://0013-glimagesink-support-video-rotation-using-transform-m.patch \
    file://0014-ion-DMA-Buf-allocator-based-on-ion.patch \
    file://0015-EGL_DMA_Buf-Wrong-attribute-list-type-for-EGL-1.5.patch \
    file://0016-glimagesink-Fix-horizontal-vertical-flip-matrizes.patch \
    file://0017-glwindow-Fix-glimagesink-cannot-show-frame-when-conn.patch \
    file://0018-ion_allocator-refine-ion-allocator-code.patch \
    file://0019-videocompositor-Remove-output-format-alpha-check.patch \
"


# Enable 'egl' packageconfig so 'glimagesink' is compiled
PACKAGECONFIG_GL_append_imxgpu3d = " ${@bb.utils.contains('DISTRO_FEATURES', 'opengl', 'egl', '', d)}"

# include fragment shaders
FILES_${PN}-opengl += "/usr/share/*.fs"

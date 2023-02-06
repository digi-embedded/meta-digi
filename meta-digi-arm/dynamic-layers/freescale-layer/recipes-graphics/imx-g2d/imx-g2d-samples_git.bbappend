# Copyright (c) 2023, Digi International Inc.

PACKAGECONFIG_IMPLEMENTATION:mx93-nxp-bsp = "pxp"

PACKAGECONFIG[dpu] = " \
    BUILD_IMPLEMENTATION=dpu, \
    , \
    imx-dpu-g2d wayland-native wayland wayland-protocols, \
    , \
    , \
    gpu-drm gpu-fbdev pxp"
PACKAGECONFIG[gpu-drm] = " \
    BUILD_IMPLEMENTATION=gpu-drm, \
    , \
    imx-gpu-g2d wayland-native wayland wayland-protocols, \
    , \
    , \
    dpu gpu-fbdev pxp"
PACKAGECONFIG[gpu-fbdev] = " \
    BUILD_IMPLEMENTATION=gpu-fbdev, \
    , \
    imx-gpu-g2d, \
    , \
    , \
    dpu gpu-drm pxp"
PACKAGECONFIG[pxp] = " \
    BUILD_IMPLEMENTATION=pxp, \
    , \
    imx-pxp-g2d wayland-native wayland wayland-protocols, \
    , \
    , \
    dpu gpu-drm gpu-fbdev"

COMPATIBLE_MACHINE = "(imxgpu2d|mx93-nxp-bsp)"

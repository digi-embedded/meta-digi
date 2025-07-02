# Copyright (C) 2025, Digi International Inc.

SOC_TOOLS_GPU_XWAYLAND:imxgpu3d = "glmark2 mesa-demos gtkperf"

DRM_TOOLS         = ""
DRM_TOOLS:imxdrm  = "kmscube"

RDEPENDS:${PN}:append:imxgpu = " ${DRM_TOOLS}"

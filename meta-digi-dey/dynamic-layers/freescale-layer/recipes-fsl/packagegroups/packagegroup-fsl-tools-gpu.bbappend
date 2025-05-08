# Copyright (C) 2025, Digi International Inc.

SOC_TOOLS_DRM ??= ""
SOC_TOOLS_DRM:imxdrm ??= "libdrm-tests"

SOC_TOOLS_GPU:append:imxgpu = " gputop"

RDEPENDS:${PN}:append = " ${SOC_TOOLS_DRM}"

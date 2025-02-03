# Copyright (C) 2025, Digi International Inc.

# We configure imx-gpu-viv driver as built-in, so there's no need to install
# the module.
RRECOMMENDS:libgal-imx:remove = "kernel-module-imx-gpu-viv"

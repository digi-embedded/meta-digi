# Copyright (C) 2023 Digi International

# We configure imx-gpu-viv driver as built-in, so there's no need to install
# the module.
RRECOMMENDS:libgal-imx:remove:ccimx6 = "kernel-module-imx-gpu-viv"

# Copyright 2023 Digi International Inc.

#
# Reuse meta-freescale's optee-client_3.19.0.imx.bb
#
require recipes-security/optee-imx/optee-client_3.19.0.imx.bb

SRCBRANCH = "lf-6.1.55_2.2.0"
SRCREV = "acb0885c117e73cb6c5c9b1dd9054cb3f93507ee"

EXTRA_OEMAKE += "PKG_CONFIG=pkg-config"

COMPATIBLE_MACHINE = "(ccimx93)"

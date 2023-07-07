# Copyright 2023 Digi International Inc.

#
# Reuse meta-freescale's optee-client_3.19.0.imx.bb
#
require recipes-security/optee-imx/optee-client_3.19.0.imx.bb

SRCBRANCH = "lf-6.1.22_2.0.0"
SRCREV = "8533e0e6329840ee96cf81b6453f257204227e6c"

# Otherwise optee-client's makefile defaults to use $(CROSS_COMPILE)pkg-config
# which is not what Yocto provides.
export PKG_CONFIG='pkg-config'

COMPATIBLE_MACHINE = "(ccimx93)"

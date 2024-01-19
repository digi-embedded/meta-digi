# Copyright 2023 Digi International Inc.

#
# Reuse meta-freescale's optee-test_3.19.0.imx.bb
#
require recipes-security/optee-imx/optee-test_3.19.0.imx.bb

DEPENDS += "openssl"

SRCBRANCH = "lf-6.1.55_2.2.0"
SRCREV = "38efacef3b14b32a6792ceaebe211b5718536fbb"

COMPATIBLE_MACHINE = "(ccimx93)"

# Copyright (C) 2024, Digi International Inc.

#
# Reuse meta-freescale's optee-test_3.19.0.imx.bb
#
require recipes-security/optee-imx/optee-test_3.19.0.imx.bb

DEPENDS += "openssl"

SRCBRANCH = "lf-6.6.23_2.0.0"
SRCREV = "07682f1b1b41ec0bfa507286979b36ab8d344a96"

COMPATIBLE_MACHINE = "(ccimx91)"

# Copyright (C) 2022, Digi International Inc.

# This is an excerpt from the *.bbappend in meta-imx, containing only the
# minimum necessary changes for bitbake to use the SDMA firmware from the
# firmware-imx recipe instead of this one

# Use the latest version of sdma firmware in firmware-imx
PACKAGES:remove = "${PN}-imx-sdma-license ${PN}-imx-sdma-imx6q ${PN}-imx-sdma-imx7d"

# Copyright (C) 2018-2020 Digi International, Inc.

# Use the v4.14 ga BSP branch
SRCBRANCH = "imx_4.14.98_2.3.0"
SRCREV = "2556000499f667123094af22326cfd8e4cbadaac"

do_deploy_append () {
    install -d ${DEPLOYDIR}/${BOOT_TOOLS}
    install -m 0755 ${S}/iMX8M/mkimage_imx8 ${DEPLOYDIR}/${BOOT_TOOLS}/mkimage_imx8m
    install -m 0755 ${S}/mkimage_imx8 ${DEPLOYDIR}/${BOOT_TOOLS}/mkimage_imx8
}

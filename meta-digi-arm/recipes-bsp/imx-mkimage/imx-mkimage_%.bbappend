# Copyright (C) 2018-2020 Digi International, Inc.

do_deploy_append () {
    install -d ${DEPLOYDIR}/${BOOT_TOOLS}
    install -m 0755 ${S}/iMX8M/mkimage_imx8 ${DEPLOYDIR}/${BOOT_TOOLS}/mkimage_imx8m
    install -m 0755 ${S}/mkimage_imx8 ${DEPLOYDIR}/${BOOT_TOOLS}/mkimage_imx8
}

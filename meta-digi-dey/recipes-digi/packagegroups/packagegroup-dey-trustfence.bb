# Copyright (C) 2016 Digi International.

SUMMARY = "DEY trustfence packagegroup"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/LICENSE;md5=3f40d7994397109285ec7b81fdeb3b58 \
                    file://${COREBASE}/meta/COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420"

inherit packagegroup

RDEPENDS_${PN} = "\
	${@base_conditional('TRUSTFENCE_CONSOLE_DISABLE', '1', 'auto-serial-console', '', d)} \
"
do_package[vardeps] += "TRUSTFENCE_CONSOLE_DISABLE"

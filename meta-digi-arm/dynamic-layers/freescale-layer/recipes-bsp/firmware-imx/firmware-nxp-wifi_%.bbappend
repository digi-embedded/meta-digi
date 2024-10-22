# Copyright (C) 2024, Digi International Inc.

LIC_FILES_CHKSUM = "file://LICENSE.txt;md5=ca53281cc0caa7e320d4945a896fb837"

SRCBRANCH = "lf-6.6.36_2.1.0"
SRCREV = "1b26d19284d202b1531837ce37a05afc49ad1d98"

FILES:${PN}-nxp8997-common += "${nonarch_base_libdir}/firmware/nxp/uart8997_bt_v4.bin"
FILES:${PN}-nxp9098-common += "${nonarch_base_libdir}/firmware/nxp/uart9098_bt_v1.bin"

# Copyright (C) 2017, 2018 Digi International Inc.

# Empirically detected binaries that are not needed for a given platform
REDUNDANT_BINS ?= ""
REDUNDANT_BINS:ccimx6ul ?= " \
    usr/lib/imx-mm/parser/lib_avi_parser_arm9_elinux* \
    usr/lib/imx-mm/parser/lib_flv_parser_arm9_elinux* \
    usr/lib/imx-mm/parser/lib_mkv_parser_arm9_elinux* \
    usr/lib/imx-mm/parser/lib_mp4_parser_arm9_elinux* \
    usr/lib/imx-mm/parser/lib_mpg2_parser_arm9_elinux* \
    usr/lib/imx-mm/parser/lib_ogg_parser_arm9_elinux* \
"

do_install:append() {
	for i in ${REDUNDANT_BINS}; do
		rm -f ${D}/${i}
	done
}

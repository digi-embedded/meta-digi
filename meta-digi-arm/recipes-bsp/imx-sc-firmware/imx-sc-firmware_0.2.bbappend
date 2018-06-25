# Copyright (C) 2018 Digi International Inc.

SRC_URI_append = " \
    file://scfw_tcm.bin \
"

do_unpack[postfuncs] += "overwrite_scfw"
overwrite_scfw () {
	# Overwrite original SCFW file from NXP
	cp -f ${WORKDIR}/scfw_tcm.bin ${S}/mx8qx-scfw-tcm.bin
}

COMPATIBLE_MACHINE = "(ccimx8x)"

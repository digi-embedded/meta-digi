# Copyright (C) 2024 Digi International

do_install:append () {
	install -d ${D}${bindir}

	# fdt_add_pubkey
	if [ -f tools/fdt_add_pubkey ]; then
		install -m 0755 tools/fdt_add_pubkey ${D}${bindir}/uboot-fdt_add_pubkey
		ln -sf uboot-fdt_add_pubkey ${D}${bindir}/fdt_add_pubkey
	fi
}

FILES:${PN}-mkimage += " \
	${bindir}/uboot-fdt_add_pubkey \
	${bindir}/fdt_add_pubkey \
"

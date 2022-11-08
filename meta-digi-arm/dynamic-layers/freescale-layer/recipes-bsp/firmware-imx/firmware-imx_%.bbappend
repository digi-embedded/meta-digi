# Copyright (C) 2022, Digi International Inc.

do_install:append() {
	# meta-freescale deletes the SDMA firmware provided by the firmware-imx package,
	# in favor of the generic one provided by the linux-firmware package. The one
	# provided by NXP is more up-to-date, so we want it back.
	install -m 0644 ${S}/firmware/sdma/* ${D}${nonarch_base_libdir}/firmware/imx/sdma
}

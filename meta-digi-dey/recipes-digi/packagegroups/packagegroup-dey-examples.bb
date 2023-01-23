# Copyright (C) 2013-2020, Digi International Inc.

SUMMARY = "DEY examples packagegroup"

DEPENDS = "virtual/kernel"

PACKAGE_ARCH = "${MACHINE_ARCH}"
inherit packagegroup

RDEPENDS:${PN} = "\
	${@bb.utils.contains("MACHINE_FEATURES", "alsa", "dey-examples-alsa", "", d)} \
	${@bb.utils.contains("MACHINE_FEATURES", "bluetooth", "dey-examples-bt", "", d)} \
	${@bb.utils.contains("MACHINE_FEATURES", "bluetooth", "dey-examples-btconfig", "", d)} \
	${@bb.utils.contains("MACHINE_FEATURES", "bluetooth", "dey-examples-bt-gatt-server", "", d)} \
	${@bb.utils.contains("MACHINE_FEATURES", "bluetooth", "dey-examples-hdp", "", d)} \
	${@bb.utils.contains("MACHINE_FEATURES", "cryptochip", "dey-examples-cryptochip", "", d)} \
	${@bb.utils.contains("MACHINE_FEATURES", "mca", "dey-examples-adc-cmp \
							 dey-examples-tamper", "", d)} \
	dey-examples-caamblob \
	dey-examples-cloudconnector \
	dey-examples-digiapix \
	dey-examples-rtc \
	connectcore-demo-example \
"
RDEPENDS:${PN}:append:ccimx6 = "\
	${@bb.utils.contains("MACHINE_FEATURES", "accel-graphics", "dey-examples-opengles", "", d)} \
	dey-examples-v4l2 \
"

COMPATIBLE_MACHINE = "(ccimx6$|ccimx6ul|ccimx8x|ccimx8m)"

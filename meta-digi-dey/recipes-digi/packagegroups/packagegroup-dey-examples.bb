# Copyright (C) 2013-2020, Digi International Inc.

SUMMARY = "DEY examples packagegroup"

DEPENDS = "virtual/kernel"

PACKAGE_ARCH = "${MACHINE_ARCH}"
inherit packagegroup

RDEPENDS_${PN} = "\
	${@bb.utils.contains("MACHINE_FEATURES", "alsa", "dey-examples-alsa", "", d)} \
	${@bb.utils.contains("MACHINE_FEATURES", "alsa", "dey-examples-vplay", "", d)} \
	${@bb.utils.contains("MACHINE_FEATURES", "bluetooth", "dey-examples-bt", "", d)} \
	${@bb.utils.contains("MACHINE_FEATURES", "bluetooth", "dey-examples-btconfig", "", d)} \
	${@bb.utils.contains("MACHINE_FEATURES", "bluetooth", "dey-examples-bt-gatt-server", "", d)} \
	${@bb.utils.contains("MACHINE_FEATURES", "bluetooth", "dey-examples-hdp", "", d)} \
	${@bb.utils.contains("MACHINE_FEATURES", "cryptochip", "dey-examples-cryptochip", "", d)} \
	awsiotsdk-demo \
	dey-examples-caamblob \
	dey-examples-cloudconnector \
	dey-examples-digiapix \
	dey-examples-rtc \
"

RDEPENDS_${PN}_append_ccimx6 = "\
	${@bb.utils.contains("MACHINE_FEATURES", "accel-graphics", "dey-examples-opengles", "", d)} \
	dey-examples-v4l2 \
"

RDEPENDS_${PN}_append_ccimx6ul = "\
	dey-examples-adc-cmp \
	dey-examples-tamper \
"

RDEPENDS_${PN}_append_ccimx8x = "\
	dey-examples-adc-cmp \
	dey-examples-tamper \
	dey-examples-v4l2 \
"

RDEPENDS_${PN}_append_ccimx8m = "\
	dey-examples-adc-cmp \
	dey-examples-tamper \
	dey-examples-v4l2 \
"

COMPATIBLE_MACHINE = "(ccimx6$|ccimx6ul|ccimx8x|ccimx8m)"

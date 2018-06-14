# Copyright (C) 2013-2018, Digi International Inc.

SUMMARY = "DEY examples packagegroup"

DEPENDS = "virtual/kernel"

PACKAGE_ARCH = "${MACHINE_ARCH}"
inherit packagegroup

RDEPENDS_${PN} = "\
	${@bb.utils.contains("MACHINE_FEATURES", "alsa", "dey-examples-alsa", "", d)} \
	${@bb.utils.contains("MACHINE_FEATURES", "alsa", "dey-examples-vplay", "", d)} \
	${@bb.utils.contains("MACHINE_FEATURES", "bluetooth", "dey-examples-bt", "", d)} \
	${@bb.utils.contains("MACHINE_FEATURES", "bluetooth", "dey-examples-btconfig", "", d)} \
	${@bb.utils.contains("MACHINE_FEATURES", "bluetooth", "dey-examples-hdp", "", d)} \
	awsiotsdk-demo \
	dey-examples-can \
	dey-examples-cloudconnector \
	dey-examples-digiapix \
	dey-examples-gpio-sysfs \
	dey-examples-rtc \
	dey-examples-spidev \
	dey-examples-watchdog \
"

RDEPENDS_${PN}_append_ccimx6 = "\
	${@bb.utils.contains("MACHINE_FEATURES", "accel-graphics", "dey-examples-opengles", "", d)} \
	dey-examples-v4l2 \
"

RDEPENDS_${PN}_append_ccimx6ul = "\
	dey-examples-adc \
	dey-examples-adc-cmp \
	dey-examples-cryptochip \
	dey-examples-tamper \
"

RDEPENDS_${PN}_append_ccimx6qpsbc = "\
	dey-examples-cryptochip \
"

RDEPENDS_${PN}_append_ccimx8x = "\
	dey-examples-adc \
	dey-examples-adc-cmp \
	dey-examples-cryptochip \
	dey-examples-tamper \
	dey-examples-v4l2 \
"

COMPATIBLE_MACHINE = "(ccimx6$|ccimx6ul|ccimx8x)"

# Copyright (C) 2013-2017 Digi International.

SUMMARY = "DEY examples packagegroup"

DEPENDS = "virtual/kernel"

PACKAGE_ARCH = "${MACHINE_ARCH}"
inherit packagegroup

RDEPENDS_${PN} = "\
	${@bb.utils.contains("MACHINE_FEATURES", "alsa", "dey-examples-alsa", "", d)} \
	dey-examples-gpio-sysfs \
	${@bb.utils.contains("MACHINE_FEATURES", "rtc", "dey-examples-rtc", "", d)} \
	dey-examples-spidev \
	${@bb.utils.contains("MACHINE_FEATURES", "alsa", "dey-examples-vplay", "", d)} \
	dey-examples-watchdog \
"

RDEPENDS_${PN}_append_ccardimx28 = "\
	${@bb.utils.contains("MACHINE_FEATURES", "bluetooth", "dey-examples-bt", "", d)} \
	${@bb.utils.contains("MACHINE_FEATURES", "bluetooth", "dey-examples-btconfig", "", d)} \
	dey-examples-can \
	${@bb.utils.contains("MACHINE_FEATURES", "bluetooth", "dey-examples-hdp", "", d)} \
"

RDEPENDS_${PN}_append_ccimx6 = "\
	awsiotsdk-demo \
	${@bb.utils.contains("MACHINE_FEATURES", "bluetooth", "dey-examples-bt", "", d)} \
	${@bb.utils.contains("MACHINE_FEATURES", "bluetooth", "dey-examples-btconfig", "", d)} \
	dey-examples-can \
	dey-examples-cloudconnector \
	${@bb.utils.contains("MACHINE_FEATURES", "bluetooth", "dey-examples-hdp", "", d)} \
	${@bb.utils.contains("MACHINE_FEATURES", "accel-graphics", "dey-examples-opengles", "", d)} \
	dey-examples-v4l2 \
"

RDEPENDS_${PN}_append_ccimx6ul = "\
	awsiotsdk-demo \
	${@bb.utils.contains("MACHINE_FEATURES", "bluetooth", "dey-examples-bt", "", d)} \
	${@bb.utils.contains("MACHINE_FEATURES", "bluetooth", "dey-examples-btconfig", "", d)} \
	dey-examples-adc \
	dey-examples-adc-cmp \
	dey-examples-can \
	dey-examples-cloudconnector \ 
	${@bb.utils.contains("MACHINE_FEATURES", "bluetooth", "dey-examples-hdp", "", d)} \
	dey-examples-tamper \
"

COMPATIBLE_MACHINE = "(ccardimx28|ccimx6$|ccimx6ul)"

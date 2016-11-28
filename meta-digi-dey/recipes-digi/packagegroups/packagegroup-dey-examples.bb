# Copyright (C) 2013 Digi International.

SUMMARY = "DEY examples packagegroup"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/LICENSE;md5=3f40d7994397109285ec7b81fdeb3b58 \
                    file://${COREBASE}/meta/COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420"

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
	${@bb.utils.contains("MACHINE_FEATURES", "bluetooth", "dey-examples-bt", "", d)} \
	${@bb.utils.contains("MACHINE_FEATURES", "bluetooth", "dey-examples-btconfig", "", d)} \
	dey-examples-can \
	${@bb.utils.contains("MACHINE_FEATURES", "bluetooth", "dey-examples-hdp", "", d)} \
	${@bb.utils.contains("MACHINE_FEATURES", "accel-graphics", "dey-examples-opengles", "", d)} \
	dey-examples-v4l2 \
"

RDEPENDS_${PN}_append_ccimx6ul = "\
	${@bb.utils.contains("MACHINE_FEATURES", "bluetooth", "dey-examples-bt", "", d)} \
	${@bb.utils.contains("MACHINE_FEATURES", "bluetooth", "dey-examples-btconfig", "", d)} \
	dey-examples-adc \
	dey-examples-can \
	${@bb.utils.contains("MACHINE_FEATURES", "bluetooth", "dey-examples-hdp", "", d)} \
	dey-examples-tamper \
"

COMPATIBLE_MACHINE = "(ccardimx28|ccimx6$|ccimx6ul)"

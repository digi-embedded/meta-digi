# Copyright (C) 2013 Digi International.

DESCRIPTION = "DEL examples packagegroup"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/LICENSE;md5=3f40d7994397109285ec7b81fdeb3b58 \
                    file://${COREBASE}/meta/COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420"

PR = "r0"

inherit packagegroup

RDEPENDS_${PN} = "\
	del-examples-adc \
	${@base_contains("MACHINE_FEATURES", "alsa", "del-examples-alsa", "", d)} \
	del-examples-gpio \
	del-examples-gpio-sysfs \
	${@base_contains("MACHINE_FEATURES", "rtc", "del-examples-rtc", "", d)} \
	del-examples-spidev \
	${@base_contains("MACHINE_FEATURES", "alsa", "del-examples-vplay", "", d)} \
	del-examples-watchdog \
"

RDEPENDS_${PN}_append_mxs = "\
	${@base_contains("MACHINE_FEATURES", "bluetooth", "del-examples-bt", "", d)} \
	${@base_contains("MACHINE_FEATURES", "bluetooth", "del-examples-btconfig", "", d)} \
	del-examples-can \
	${@base_contains("MACHINE_FEATURES", "bluetooth", "del-examples-hdp", "", d)} \
"

RDEPENDS_${PN}_append_mx5 = "\
	${@base_contains("MACHINE_FEATURES", "accelerometer", "del-examples-accelerometer", "", d)} \
	${@base_contains("MACHINE_FEATURES", "accel-graphics", "del-examples-opengles", "", d)} \
	del-examples-sahara \
	del-examples-v4l2 \
"

RDEPENDS_${PN}_append_ccimx51js_mx5 = "\
	del-examples-battery \
"

RDEPENDS_${PN}_append_ccimx53js_mx5 = "\
	del-examples-can \
"

PACKAGE_ARCH = "${MACHINE_ARCH}"

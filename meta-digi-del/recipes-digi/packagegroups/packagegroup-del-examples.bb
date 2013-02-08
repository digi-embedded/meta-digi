DESCRIPTION = "DEL examples packagegroup"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/LICENSE;md5=3f40d7994397109285ec7b81fdeb3b58 \
                    file://${COREBASE}/meta/COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420"

PR = "r0"

inherit packagegroup

RDEPENDS_${PN} = "\
	del-examples-adc \
	del-examples-alsa \
	del-examples-gpio \
	del-examples-gpio-sysfs \
	del-examples-rtc \
	del-examples-spidev \
	del-examples-vplay \
	del-examples-watchdog \
"

RDEPENDS_${PN}_append_ccardxmx28js = "\
	del-examples-bt \
	del-examples-btconfig \
	del-examples-can \
	del-examples-hdp \
"

RDEPENDS_${PN}_append_ccxmx51js = "\
	del-examples-accelerometer \
	del-examples-battery \
	del-examples-opengles \
	del-examples-sahara \
	del-examples-v4l2 \
"

RDEPENDS_${PN}_append_ccxmx53js = "\
	del-examples-accelerometer \
	del-examples-can \
	del-examples-opengles \
	del-examples-sahara \
	del-examples-v4l2 \
"

PACKAGE_ARCH = "${MACHINE_ARCH}"

DESCRIPTION = "Example GPIO kernel module."
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=12f884d2ae1ff87c09e5b7ccc2c4ca7e"

inherit module

PR = "r0"
PV = "2.1"

SRC_URI = "\
	file://Makefile \
	file://gpio.c \
	file://gpio.h \
	file://COPYING \
	"

S = "${WORKDIR}"

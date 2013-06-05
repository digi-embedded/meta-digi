#
# Copyright (C) 2012 Digi International.
#
SUMMARY = "Core packagegroup for DEY image"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/LICENSE;md5=3f40d7994397109285ec7b81fdeb3b58"
PACKAGE_ARCH = "${MACHINE_ARCH}"
DEPENDS = "virtual/kernel"
ALLOW_EMPTY = "1"
PR = "r0"

inherit packagegroup

#
# Set by the machine configuration with packages essential for device bootup
#
MACHINE_ESSENTIAL_EXTRA_RDEPENDS ?= ""
MACHINE_ESSENTIAL_EXTRA_RRECOMMENDS ?= ""

# Distro can override the following VIRTUAL-RUNTIME providers:
VIRTUAL-RUNTIME_login_manager ?= ""
VIRTUAL-RUNTIME_passwd_manager ?= "shadow"
VIRTUAL-RUNTIME_init_manager ?= "sysvinit"
VIRTUAL-RUNTIME_initscripts ?= "initscripts"
VIRTUAL-RUNTIME_keymaps ?= "keymaps"

# Set device manager depending on X11 feature
VIRTUAL-RUNTIME_dev_manager ?= "${@base_contains('DISTRO_FEATURES', 'x11', 'udev', 'busybox-mdev', d)}"

RDEPENDS_${PN} = "\
    base-files \
    base-passwd \
    busybox \
    ${@base_contains("MACHINE_FEATURES", "rtc", "busybox-hwclock", "", d)} \
    ${@base_contains("MACHINE_FEATURES", "keyboard", "${VIRTUAL-RUNTIME_keymaps}", "", d)} \
    ${@base_contains("MACHINE_FEATURES", "touchscreen", "tslib tslib-calibrate tslib-tests", "",d)} \
    modutils-initscripts \
    netbase \
    nvram \
    ${VIRTUAL-RUNTIME_dev_manager} \
    ${VIRTUAL-RUNTIME_init_manager} \
    ${VIRTUAL-RUNTIME_initscripts} \
    ${VIRTUAL-RUNTIME_login_manager} \
    ${VIRTUAL-RUNTIME_passwd_manager} \
    ${VIRTUAL-RUNTIME_update-alternatives} \
    ubootenv \
    update-flash \
    usbutils \
    ${@base_contains("MACHINE_FEATURES", "usbgadget", "kernel-module-g-ether kernel-module-g-file-storage kernel-module-g-serial", "",d)} \
    ${MACHINE_ESSENTIAL_EXTRA_RDEPENDS}"

RRECOMMENDS_${PN} = "\
    ${MACHINE_ESSENTIAL_EXTRA_RRECOMMENDS}"

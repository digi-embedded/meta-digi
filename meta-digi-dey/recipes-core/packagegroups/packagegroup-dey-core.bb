#
# Copyright (C) 2012-2017 Digi International.
#
SUMMARY = "Core packagegroup for DEY image"

DEPENDS = "virtual/kernel"

PACKAGE_ARCH = "${MACHINE_ARCH}"
inherit packagegroup

#
# Set by the machine configuration with packages essential for device bootup
#
MACHINE_ESSENTIAL_EXTRA_RDEPENDS ?= ""
MACHINE_ESSENTIAL_EXTRA_RRECOMMENDS ?= ""

# Distro can override the following VIRTUAL-RUNTIME providers:
VIRTUAL-RUNTIME_base-utils ?= "busybox"
VIRTUAL-RUNTIME_base-utils-acpid ?= "busybox-acpid"
VIRTUAL-RUNTIME_base-utils-hwclock ?= "busybox-hwclock"
VIRTUAL-RUNTIME_base-utils-syslog ?= "busybox-syslog"
VIRTUAL-RUNTIME_dev_manager ?= "udev"
VIRTUAL-RUNTIME_init_manager ?= "sysvinit"
VIRTUAL-RUNTIME_initscripts ?= "initscripts"
VIRTUAL-RUNTIME_keymaps ?= "keymaps"
VIRTUAL-RUNTIME_login_manager ?= ""
VIRTUAL-RUNTIME_passwd_manager ?= "shadow"

# Set virtual runtimes depending on X11 feature
VIRTUAL-RUNTIME_touchscreen ?= "${@bb.utils.contains('DISTRO_FEATURES', 'x11', '', 'tslib-calibrate tslib-tests', d)}"

RDEPENDS:${PN} = "\
    awsiotsdk-c \
    base-files \
    base-passwd \
    connectcore-demo-example \
    cloudconnector \
    cryptodev-module \
    ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'firmwared', '',d)} \
    ${@bb.utils.contains("MACHINE_FEATURES", "keyboard", "${VIRTUAL-RUNTIME_keymaps}", "", d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', '', bb.utils.contains("MACHINE_FEATURES", "rtc", "${VIRTUAL-RUNTIME_base-utils-hwclock}", "", d), d)} \
    ${@bb.utils.contains("MACHINE_FEATURES", "touchscreen", "${VIRTUAL-RUNTIME_touchscreen}", "",d)} \
    init-ifupdown \
    libdigiapix \
    libgpiod \
    libgpiod-tools \
    libubootenv-bin \
    modutils-initscripts \
    netbase \
    networkmanager \
    networkmanager-nmcli \
    os-release \
    ${@bb.utils.contains('MACHINE_FEATURES', 'pci', 'pciutils', '',d)} \
    recovery-utils \
    sysinfo \
    ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'system-monitor', '',d)} \
    usbutils \
    ${VIRTUAL-RUNTIME_base-utils} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', '', '${VIRTUAL-RUNTIME_base-utils-acpid}', d)} \
    ${VIRTUAL-RUNTIME_dev_manager} \
    ${VIRTUAL-RUNTIME_init_manager} \
    ${VIRTUAL-RUNTIME_initscripts} \
    ${VIRTUAL-RUNTIME_login_manager} \
    ${VIRTUAL-RUNTIME_passwd_manager} \
    ${VIRTUAL-RUNTIME_update-alternatives} \
    ${MACHINE_ESSENTIAL_EXTRA_RDEPENDS} \
    ${MACHINE_EXTRA_RDEPENDS} \
"

RRECOMMENDS:${PN} = "\
    ${VIRTUAL-RUNTIME_base-utils-syslog} \
    ${MACHINE_ESSENTIAL_EXTRA_RRECOMMENDS} \
    ${MACHINE_EXTRA_RRECOMMENDS} \
"

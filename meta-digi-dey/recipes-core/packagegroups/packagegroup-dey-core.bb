#
# Copyright (C) 2012-2023 Digi International.
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
VIRTUAL-RUNTIME_keymaps ?= "keymaps"
VIRTUAL-RUNTIME_passwd_manager ?= "shadow"

# Set virtual runtimes depending on X11 feature
VIRTUAL-RUNTIME_touchscreen ?= "${@bb.utils.contains('DISTRO_FEATURES', 'x11', '', 'tslib-calibrate tslib-tests', d)}"

SYSVINIT_SCRIPTS = " \
    ${@bb.utils.contains('MACHINE_FEATURES', 'rtc', '${VIRTUAL-RUNTIME_base-utils-hwclock}', '', d)} \
    modutils-initscripts \
    ${VIRTUAL-RUNTIME_base-utils-acpid} \
    ${VIRTUAL-RUNTIME_initscripts} \
"

RDEPENDS:${PN} = "\
    base-files \
    base-passwd \
    bootcount \
    connectcore-demo-example \
    ${CCCS_PKGS} \
    dualboot \
    ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'firmwared', '',d)} \
    ${@bb.utils.contains("MACHINE_FEATURES", "keyboard", "${VIRTUAL-RUNTIME_keymaps}", "", d)} \
    ${@bb.utils.contains("MACHINE_FEATURES", "touchscreen", "${VIRTUAL-RUNTIME_touchscreen}", "",d)} \
    init-ifupdown \
    libdigiapix \
    libgpiod \
    libgpiod-tools \
    libubootenv-bin \
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
    ${@bb.utils.contains('DISTRO_FEATURES', 'sysvinit', '${SYSVINIT_SCRIPTS}', '', d)} \
    ${VIRTUAL-RUNTIME_dev_manager} \
    ${VIRTUAL-RUNTIME_init_manager} \
    ${VIRTUAL-RUNTIME_login_manager} \
    ${VIRTUAL-RUNTIME_passwd_manager} \
    ${VIRTUAL-RUNTIME_update-alternatives} \
    ${MACHINE_ESSENTIAL_EXTRA_RDEPENDS} \
    ${MACHINE_EXTRA_RDEPENDS} \
"

RDEPENDS:${PN}:append:ccmp15 = " \
    v4l-utils \
"

# The rootfs in the CC6UL is not big enough for graphic images (QT) and the
# connectcore demo, so we restrict the demo only for the 'core-image-base'
RDEPENDS:${PN}:remove:ccimx6ul = "connectcore-demo-example"

RRECOMMENDS:${PN} = "\
    ${VIRTUAL-RUNTIME_base-utils-syslog} \
    ${MACHINE_ESSENTIAL_EXTRA_RRECOMMENDS} \
    ${MACHINE_EXTRA_RRECOMMENDS} \
"

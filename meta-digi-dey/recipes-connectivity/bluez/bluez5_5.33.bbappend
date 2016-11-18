# Copyright (C) 2015 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BP}:"

SRC_URI += " \
    file://bluez-init \
    file://0021-hcitool-do-not-show-unsupported-refresh-option.patch \
    file://0022-hcitool-increase-the-shown-connection-limit-to-20.patch \
"

SRC_URI_append_ccimx6ul = " \
    file://0001-bluetooth-Add-bluetooth-support-for-QCA6174-chip.patch \
    file://0002-bluetooth-Enable-bluetooth-low-power-mode-functional.patch \
    file://0003-bluetooth-Fix-bug-in-firmware-parsing-mechanism.patch \
    file://0004-bluetooth-Configure-BD-Address.patch \
    file://0005-bluetooth-Remove-unused-functions-in-the-firmware-do.patch \
    file://0006-bluetooth-Enable-3Mbps-baud-rate-support.patch \
    file://0007-bluetooth-Check-TTY-buffer-for-data-availability-bef.patch \
    file://0008-bluetooth-Add-support-for-TUFEELO-firmware-download.patch \
    file://0009-bluetooth-Add-support-for-ROME-3.2-SOC.patch \
    file://0010-bluetooth-Use-correct-TTY-ioctl-calls-for-flow-contr.patch \
    file://0011-bluetooth-Add-support-for-multi-baud-rate.patch \
    file://0012-Override-PCM-Settings-by-reading-configuration-file.patch \
    file://0013-Add-support-for-Tufello-1.1-SOC.patch \
    file://0014-bluetooth-Vote-UART-CLK-ON-prior-to-firmware-downloa.patch \
    file://0015-Override-IBS-settings-by-reading-configuration-file.patch \
    file://0016-Handle-NULL-Pointer-derefrencing-in-AVRCP-Target-rol.patch \
    file://0017-bluetooth-Fix-flow-control-operation.patch \
    file://0018-Adding-MDM-specific-code-under-_PLATFORM_MDM_.patch \
    file://0019-Bluetooth-Fix-static-analysis-issues.patch \
    file://0023-hciattach_rome-do-not-override-the-baudrate-in-the-N.patch \
"

inherit update-rc.d

PACKAGECONFIG_append = " experimental"

do_install_append() {
	install -d ${D}${sysconfdir}/init.d/
	install -m 0755 ${WORKDIR}/bluez-init ${D}${sysconfdir}/init.d/bluez
}

INITSCRIPT_NAME = "bluez"
INITSCRIPT_PARAMS = "start 10 5 ."

PACKAGE_ARCH = "${MACHINE_ARCH}"

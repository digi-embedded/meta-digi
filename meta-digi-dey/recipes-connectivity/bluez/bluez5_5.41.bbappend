# Copyright (C) 2015-2017 Digi International.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BP}:"

SRC_URI += " \
    file://bluetooth-init \
    file://main.conf \
    file://0001-hcitool-do-not-show-unsupported-refresh-option.patch \
    file://0002-hcitool-increase-the-shown-connection-limit-to-20.patch \
    file://0025-port-test-discovery-to-python3.patch \
    file://0027-example-gatt-server-update-example-to-master-version.patch \
"

SRC_URI_append_ccimx6ul = " \
    file://0003-bluetooth-Add-bluetooth-support-for-QCA6174-chip.patch \
    file://0004-bluetooth-Enable-bluetooth-low-power-mode-functional.patch \
    file://0005-bluetooth-Fix-bug-in-firmware-parsing-mechanism.patch \
    file://0006-bluetooth-Configure-BD-Address.patch \
    file://0007-bluetooth-Remove-unused-functions-in-the-firmware-do.patch \
    file://0008-bluetooth-Enable-3Mbps-baud-rate-support.patch \
    file://0009-bluetooth-Check-TTY-buffer-for-data-availability-bef.patch \
    file://0010-bluetooth-Add-support-for-TUFEELO-firmware-download.patch \
    file://0011-bluetooth-Add-support-for-ROME-3.2-SOC.patch \
    file://0012-bluetooth-Use-correct-TTY-ioctl-calls-for-flow-contr.patch \
    file://0013-bluetooth-Add-support-for-multi-baud-rate.patch \
    file://0014-Override-PCM-Settings-by-reading-configuration-file.patch \
    file://0015-Add-support-for-Tufello-1.1-SOC.patch \
    file://0016-bluetooth-Vote-UART-CLK-ON-prior-to-firmware-downloa.patch \
    file://0017-Override-IBS-settings-by-reading-configuration-file.patch \
    file://0018-Handle-NULL-Pointer-derefrencing-in-AVRCP-Target-rol.patch \
    file://0019-bluetooth-Fix-flow-control-operation.patch \
    file://0020-Adding-MDM-specific-code-under-_PLATFORM_MDM_.patch \
    file://0021-Bluetooth-Fix-static-analysis-issues.patch \
    file://0022-hciattach_rome-Respect-the-user-indication-for-noflo.patch \
    file://0023-hciattach-If-the-user-supplies-a-bdaddr-use-it.patch \
    file://0024-hciattach-Add-verbosity-option.patch \
"

inherit update-rc.d

PACKAGECONFIG_append = " experimental"

do_install_append() {
	install -d ${D}${sysconfdir}/init.d/
	install -m 0755 ${WORKDIR}/bluetooth-init ${D}${sysconfdir}/init.d/bluetooth-init
	install -m 0644 ${WORKDIR}/main.conf ${D}${sysconfdir}/bluetooth/
	if [ -n "${@bb.utils.contains('PACKAGECONFIG', 'experimental', 'experimental', '', d)}" ]; then
		sed -i '/^SSD_OPTIONS/a SSD_OPTIONS="${SSD_OPTIONS} --experimental"' ${D}${INIT_D_DIR}/bluetooth
	fi
}

PACKAGES =+ "${PN}-init"

FILES_${PN} += " ${sysconfdir}/bluetooth/main.conf"
FILES_${PN}-init = "${sysconfdir}/init.d/bluetooth-init"

INITSCRIPT_PACKAGES += "${PN}-init"
INITSCRIPT_NAME_${PN}-init = "bluetooth-init"
INITSCRIPT_PARAMS_${PN}-init = "start 19 2 3 4 5 . stop 21 0 1 6 ."

PACKAGE_ARCH = "${MACHINE_ARCH}"

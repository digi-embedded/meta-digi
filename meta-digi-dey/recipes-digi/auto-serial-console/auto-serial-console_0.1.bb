SUMMARY = "Auto Serial Console script"
DESCRIPTION = "Scripts to call the console tty from the kernel cmd line"
SECTION = "base"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "\
    file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302 \
    file://${COREBASE}/meta/COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420 \
"

AUTOGETTY_FILE="${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'auto-getty-systemd', 'auto-getty-sysvinit', d)}"

SRC_URI = "\
    file://autogetty \
    file://${AUTOGETTY_FILE} \
    file://auto-getty.service \
    file://auto-serial-console \
"

S = "${WORKDIR}"

inherit systemd update-rc.d

INITSCRIPT_NAME = "auto-serial-console"
INITSCRIPT_PARAMS = "start 99 5 ."
SYSTEMD_SERVICE:${PN} = "auto-getty.service"

do_install () {
	install -m 0755 -d ${D}${sysconfdir}/default
	AUTOGETTY_ENABLE='${@oe.utils.conditional( "TRUSTFENCE_CONSOLE_DISABLE", "1", "1", "0", d )}'
	install -m 0644 ${WORKDIR}/autogetty ${D}${sysconfdir}/default/autogetty
	sed -i -e "s/##ENABLED##/${AUTOGETTY_ENABLE}/g" ${D}${sysconfdir}/default/autogetty

	install -m 0755 -d ${D}${sysconfdir}/init.d
	install -m 0755 ${WORKDIR}/auto-serial-console ${D}${sysconfdir}/init.d/auto-serial-console

	install -d ${D}${systemd_unitdir}/system/
	install -m 0644 ${WORKDIR}/auto-getty.service ${D}${systemd_unitdir}/system/auto-getty.service

	install -m 0755 -d ${D}${bindir}
	install -m 0755 ${WORKDIR}/${AUTOGETTY_FILE} ${D}${bindir}/auto-getty
}

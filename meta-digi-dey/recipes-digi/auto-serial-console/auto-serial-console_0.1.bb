SUMMARY = "Auto Serial Console script"
DESCRIPTION = "Scripts to call the console tty from the kernel cmd line"
SECTION = "base"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "\
    file://${COREBASE}/LICENSE;md5=4d92cd373abda3937c2bc47fbc49d690 \
    file://${COREBASE}/meta/COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420 \
"

SRC_URI = "\
    file://autogetty \
    file://auto-getty \
    file://auto-serial-console \
"

S = "${WORKDIR}"

inherit update-rc.d

INITSCRIPT_NAME = "auto-serial-console"
INITSCRIPT_PARAMS = "start 99 5 ."

do_install () {
	install -m 0755 -d ${D}${sysconfdir}/default
	AUTOGETTY_ENABLE='${@base_conditional( "TRUSTFENCE_CONSOLE_DISABLE", "1", "1", "0", d )}'
	install -m 0644 ${WORKDIR}/autogetty ${D}${sysconfdir}/default/autogetty
	sed -i -e "s/##ENABLED##/${AUTOGETTY_ENABLE}/g" ${D}${sysconfdir}/default/autogetty

	install -m 0755 -d ${D}${sysconfdir}/init.d
	install -m 0755 ${WORKDIR}/auto-serial-console ${D}${sysconfdir}/init.d/auto-serial-console

	install -m 0755 -d ${D}${bindir}
	install -m 0755 ${WORKDIR}/auto-getty ${D}${bindir}/auto-getty
}

SUMMARY = "RDP & SSH Session Updater"
DESCRIPTION = "A Python script to send RDP and SSH session statuses to the remote lab server."
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://rdp_updater.py \
           file://rdp_updater.service"

S = "${WORKDIR}"

inherit systemd

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${WORKDIR}/rdp_updater.py ${D}${bindir}/rdp_updater

    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${WORKDIR}/rdp_updater.service ${D}${systemd_system_unitdir}/rdp_updater.service
}

SYSTEMD_SERVICE:${PN} = "rdp_updater.service"

FILES:${PN} = "${bindir}/rdp_updater \
               ${systemd_system_unitdir}/rdp_updater.service"

RDEPENDS:${PN} = "python3 python3-requests"

PROVIDES += "rdp_updater"

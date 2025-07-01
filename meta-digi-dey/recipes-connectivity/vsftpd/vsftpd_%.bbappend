# Copyright (C) 2013-2025, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI:append = " \
	${@bb.utils.contains('PACKAGECONFIG', 'openssl', 'file://0001-builddefs-add-support-to-OpenSSL.patch', '', d)} \
	${@bb.utils.contains('PACKAGECONFIG', 'openssl', 'file://vsftpd-cert', '', d)} \
	${@bb.utils.contains('PACKAGECONFIG', 'openssl', 'file://vsftpd-cert.service', '', d)} \
"

RDEPENDS:${PN}:append = "${@bb.utils.contains('PACKAGECONFIG', 'openssl', ' ${PN}-cert', '', d)}"

PACKAGECONFIG:append = " openssl "
PACKAGECONFIG[openssl] = ",,openssl"

LDFLAGS += "${@bb.utils.contains('PACKAGECONFIG', 'openssl', '-lssl -lcrypto', '', d)}"

VSFTPD_PEM ?= "vsftpd.pem"
VSFTPD_KEY ?= "vsftpd.key"
# args to openssl req (Default is -batch for non interactive mode and
# -newkey for new certificate)
VSFTPD_KEY_REQ_ARGS ?= "-nodes -batch -newkey rsa:2048"
# Standard format for public key certificate
VSFTPD_KEY_SIGN_PKCS ?= "-x509"

do_install:append() {
    if ! test -z "${PAMLIB}" ; then
        # Access through Pluggable Authentication Modules (PAM)
        echo "pam_service_name=vsftpd" >> ${D}${sysconfdir}/vsftpd.conf
    fi
    if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
        install -d ${D}${sysconfdir}/tmpfiles.d
        echo "d /run/vsftpd/empty 0755 root root -" \
        > ${D}${sysconfdir}/tmpfiles.d/${BPN}.conf
    fi
    if ${@bb.utils.contains('PACKAGECONFIG', 'openssl', 'true', 'false', d)}; then
        VSFTPD_PEM_BASE_NAME=$(basename ${VSFTPD_PEM})
        VSFTPD_KEY_BASE_NAME=$(basename ${VSFTPD_KEY})
        #  Install user certificate if provided
        if [ -f "${VSFTPD_PEM}" ] && [ -f "${VSFTPD_KEY}" ]; then
            install -m 0644 ${VSFTPD_PEM} ${D}${sysconfdir}/${VSFTPD_PEM_BASE_NAME}
            install -m 0400 ${VSFTPD_KEY} ${D}${sysconfdir}/${VSFTPD_KEY_BASE_NAME}
        fi

        # Install systemd service
        if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
            # Install systemd unit files
            install -d ${D}${systemd_unitdir}/system
            install -m 0644 ${WORKDIR}/vsftpd-cert.service ${D}${systemd_unitdir}/system/
            sed -i -e "s@##VSFTPD_PEM##@${VSFTPD_PEM_BASE_NAME}@g" \
                   "${D}${systemd_unitdir}/system/vsftpd-cert.service"
        fi

        # Install init script to generate certificate on target
        install -d ${D}${sysconfdir}/init.d
        install -m 0755 ${WORKDIR}/vsftpd-cert ${D}${sysconfdir}/vsftpd-cert
        sed -i -e "s@##VSFTPD_PEM##@${VSFTPD_PEM_BASE_NAME}@g" \
               -e "s@##VSFTPD_KEY##@${VSFTPD_KEY_BASE_NAME}@g" \
               -e "s@##VSFTPD_KEY_SIGN_PKCS##@${VSFTPD_KEY_SIGN_PKCS}@g" \
               -e "s@##VSFTPD_KEY_REQ_ARGS##@${VSFTPD_KEY_REQ_ARGS}@g" \
               "${D}${sysconfdir}/vsftpd-cert"
        ln -sf ${sysconfdir}/vsftpd-cert ${D}${sysconfdir}/init.d/vsftpd-cert

        # Customize vsftpd.conf
        sed -i -e "s@##VSFTPD_PEM##@${VSFTPD_PEM_BASE_NAME}@g" \
               -e "s@##VSFTPD_KEY##@${VSFTPD_KEY_BASE_NAME}@g" \
               "${D}${sysconfdir}/vsftpd.conf"
    fi
}

PACKAGES =+ "${PN}-cert"
FILES:${PN}-cert = " \
    ${sysconfdir}/vsftpd-cert \
    ${sysconfdir}/init.d/vsftpd-cert \
    ${systemd_unitdir}/system/vsftpd-cert.service \
"

INITSCRIPT_PACKAGES += "${@bb.utils.contains('PACKAGECONFIG', 'openssl', '${PN}-cert', '', d)}"
INITSCRIPT_NAME:${PN}-cert = "vsftpd-cert"
INITSCRIPT_PARAMS:${PN}-cert = "start 70 3 5 . stop 20 0 1 2 6 ."

SYSTEMD_PACKAGES += "${@bb.utils.contains('PACKAGECONFIG', 'openssl', '${PN}-cert', '', d)}"
SYSTEMD_SERVICE:${PN}-cert = "vsftpd-cert.service"

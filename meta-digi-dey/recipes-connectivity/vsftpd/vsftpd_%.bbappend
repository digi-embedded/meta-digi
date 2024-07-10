# Copyright (C) 2013-2019, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

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
}

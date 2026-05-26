# Copyright (C) 2026, Digi International Inc.

FILESEXTRAPATHS:prepend := "${COREBASE}/../meta-virtualization/recipes-containers/lxc/files:"

require recipes-containers/lxc/lxc_git.bb
require recipes-containers/lxc/lxc-cc-container-path.inc

DESCRIPTION = "Trimmed LXC runtime for dey-image-containers"

FILES:${PN} += "${libdir}/lxc/rootfs"

RDEPENDS:${PN}:remove = " \
    rsync curl gzip xz tar \
    bridge-utils dnsmasq \
    gmp libidn gnutls nettle \
    perl-module-strict perl-module-getopt-long perl-module-vars \
    perl-module-exporter perl-module-constant perl-module-overload \
    perl-module-exporter-heavy \
"

PACKAGECONFIG:remove = "templates systemd selinux"

RCONFLICTS:${PN}:append = " lxc"

SYSTEMD_PACKAGES = ""
SYSTEMD_SERVICE:${PN} = ""
SYSTEMD_SERVICE:${PN}-networking = ""

do_install:append() {
    for bin in \
        lxc-autostart \
        lxc-cgroup \
        lxc-checkconfig \
        lxc-checkpoint \
        lxc-config \
        lxc-console \
        lxc-copy \
        lxc-create \
        lxc-device \
        lxc-execute \
        lxc-freeze \
        lxc-monitor \
        lxc-snapshot \
        lxc-top \
        lxc-unfreeze \
        lxc-unshare \
        lxc-update-config \
        lxc-usernsexec \
        lxc-wait \
    ; do
        rm -f ${D}${bindir}/$bin
    done

    rm -f ${D}${datadir}/lxc/lxc-patch.py
    rm -f ${D}${datadir}/lxc/lxc.functions
    rm -f ${D}${libexecdir}/lxc/lxc-apparmor-load
    rm -f ${D}${libexecdir}/lxc/lxc-containers
    rm -f ${D}${libexecdir}/lxc/lxc-net
    rm -f ${D}${libexecdir}/lxc/lxc-user-nic
    rm -f ${D}${sbindir}/init.lxc
    rm -f ${D}${sbindir}/init.lxc.static
    rm -f ${D}${sysconfdir}/default/lxc
    rm -f ${D}${sysconfdir}/default/volatiles/99_lxc
    rm -f ${D}${sysconfdir}/init.d/lxc-containers
    rm -f ${D}${sysconfdir}/lxc/default.conf

    rm -rf ${D}${datadir}/bash-completion
    rm -rf ${D}${datadir}/doc
    rm -rf ${D}${datadir}/lxc/config
    rm -rf ${D}${datadir}/lxc/hooks
    rm -rf ${D}${libexecdir}/lxc/hooks
    rm -rf ${D}${sysconfdir}/dnsmasq.d

    rmdir ${D}${sbindir} 2>/dev/null || true
}

# Copyright (C) 2026, Digi International Inc.

FILESEXTRAPATHS:prepend := "${COREBASE}/../meta-virtualization/recipes-containers/podman/podman:"

require recipes-containers/podman/podman_git.bb

DESCRIPTION = "Trimmed Podman runtime for dey-image-container-manager"

# Enable Podman bridge networking with netavark
VIRTUAL-RUNTIME_container_networking = "netavark"

RDEPENDS:${PN}:append = " \
    netavark \
    aardvark-dns \
    iptables-modules \
"

RDEPENDS:${PN}:remove = " \
    libdevmapper \
"

PACKAGECONFIG:remove = "rootless docker"

RCONFLICTS:${PN}:append = " podman"

do_install:append() {
    rm -f ${D}${bindir}/docker
    rm -f ${D}${bindir}/docker-runc
    rm -f ${D}${bindir}/podman-remote
    rm -f ${D}${bindir}/podmansh
    rm -f ${D}${libexecdir}/podman/quadlet
    rm -f ${D}${libexecdir}/podman/rootlessport
    rm -f ${D}${sysconfdir}/profile.d/podman-docker.csh
    rm -f ${D}${sysconfdir}/profile.d/podman-docker.sh
    rm -rf ${D}${systemd_system_unitdir}
    rm -rf ${D}${systemd_user_unitdir}
    rm -rf ${D}${nonarch_libdir}
    rm -rf ${D}${datadir}/user-tmpfiles.d
}

SYSTEMD_SERVICE:${PN} = ""

FILES:${PN}:remove = " \
    ${systemd_system_unitdir} \
    ${systemd_user_unitdir} \
    ${nonarch_libdir} \
"

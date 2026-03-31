# Copyright (C) 2026, Digi International Inc.

FILESEXTRAPATHS:prepend := "${COREBASE}/../meta-virtualization/recipes-containers/podman/podman:"

require recipes-containers/podman/podman_git.bb

DESCRIPTION = "Trimmed Podman runtime for dey-image-container-manager"

# No CNI implies no isolated or custom network support, no NAT and no forwarding.
# Host networking still works, and Podman uses netavark/aardvark-dns in this setup.
VIRTUAL-RUNTIME_container_networking = ""
PODMAN_NETWORK_BACKEND = "netavark"

RDEPENDS:${PN}:append = " \
    netavark \
    aardvark-dns \
"

RDEPENDS:${PN}:remove = " \
    iptables \
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

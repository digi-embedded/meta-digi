# Copyright (C) 2026, Digi International Inc.

PODMAN_NETWORK_BACKEND ?= "${@('netavark' if d.getVar('VIRTUAL-RUNTIME_container_networking') == 'netavark' else 'cni')}"

do_install:append() {
    install -d ${D}${sysconfdir}/containers
    cat > ${D}${sysconfdir}/containers/containers.conf <<EOF
[network]
network_backend="${PODMAN_NETWORK_BACKEND}"
EOF
}

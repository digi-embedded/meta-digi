# Copyright (C) 2026, Digi International Inc.

do_install:append() {
    install -d ${D}${sysconfdir}/lxc
    lxc_conf="${D}${sysconfdir}/lxc/lxc.conf"
    touch "$lxc_conf"

    if grep -q '^[[:space:]]*lxc\.lxcpath[[:space:]]*=' "$lxc_conf"; then
        sed -i \
            's|^[[:space:]]*lxc\.lxcpath[[:space:]]*=.*|lxc.lxcpath = ${CC_CONTAINER_PATH}/installed/lxc|' \
            "$lxc_conf"
    else
        printf '%s\n' 'lxc.lxcpath = ${CC_CONTAINER_PATH}/installed/lxc' >> "$lxc_conf"
    fi
}

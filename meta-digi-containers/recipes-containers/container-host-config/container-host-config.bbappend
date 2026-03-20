do_install:append() {
    if [ -f ${D}${sysconfdir}/containers/storage.conf ]; then
        sed -i 's|^graphroot = ".*"|graphroot = "/mnt/data/cc-container/storage"|' \
            ${D}${sysconfdir}/containers/storage.conf
    fi
}

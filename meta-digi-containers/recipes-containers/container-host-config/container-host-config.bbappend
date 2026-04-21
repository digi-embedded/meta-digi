do_install:append() {
    if [ -f ${D}${sysconfdir}/containers/storage.conf ]; then
        sed -i 's|^graphroot = ".*"|graphroot = "${CC_CONTAINER_PATH}/storage"|' \
            ${D}${sysconfdir}/containers/storage.conf
    fi
}

FILESEXTRAPATHS_prepend_del := "${THISDIR}/files"
SRC_URI_append_del += "file://defconfig-del"
PR_append_del = "+${DISTRO}.0"

do_configure_prepend_del () {
        cp ${WORKDIR}/defconfig-del ${WORKDIR}/defconfig
}

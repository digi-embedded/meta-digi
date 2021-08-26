FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI += " ${@bb.utils.contains('DISTRO_FEATURES', 'selinux', 'file://selinux.cfg', '', d)}"

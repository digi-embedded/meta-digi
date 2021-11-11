IMAGE_INSTALL_append = " ${@bb.utils.contains('DISTRO_FEATURES', 'selinux', 'packagegroup-core-selinux', '', d)}"

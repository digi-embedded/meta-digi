IMAGE_INSTALL:append = " ${@bb.utils.contains('DISTRO_FEATURES', 'selinux', 'packagegroup-core-selinux', '', d)}"

require recipes-graphics/imx-gpu-g2d/imx-gpu-g2d_6.4.3.p1.0.bb

LIC_FILES_CHKSUM = "file://COPYING;md5=a632fefd1c359980434f9389833cab3a" 

FSLBIN_NAME_arm = "${PN}-${PV}-${TARGET_ARCH}"

SRC_URI[aarch64.md5sum] = "7360943e3027d88f583d85ea9f2b6e3d"
SRC_URI[aarch64.sha256sum] = "1b0369e9f75bf8fff7b8a86ade8d0d1748a52579b02ef72e53ad5565eee4dc60"
SRC_URI[arm.md5sum] = "20db68db8556170db0b955cc6771555d"
SRC_URI[arm.sha256sum] = "9fd1b2c05ecabb3cc1468c357fc85f024a736de54abb9942e17230f24fa280c0"

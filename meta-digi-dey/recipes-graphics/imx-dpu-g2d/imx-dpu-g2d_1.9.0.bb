require recipes-graphics/imx-dpu-g2d/imx-dpu-g2d_1.8.12.bb

LIC_FILES_CHKSUM = "file://COPYING;md5=a632fefd1c359980434f9389833cab3a"

SRC_URI[md5sum] = "22130817f758bcb844aa8495e53b24f1"
SRC_URI[sha256sum] = "96d40b1a27c8fda1465d24ad5e402d357956f9442b4e14e816efcd60e48a9874"

RDEPENDS_${PN} += "libopencl-imx"

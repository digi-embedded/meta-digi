require recipes-browser/wpewebkit/wpewebkit.inc

SRC_URI = "https://wpewebkit.org/releases/${P}.tar.xz"
SRC_URI[md5sum] = "1dd3f56b8eba16266166d757acb979fc"
SRC_URI[sha1sum] = "c5b3a48d886375a6982dd2dc5c9cc2f92f5a9690"
SRC_URI[sha256sum] = "7b6b39a12ccf3f84da4cc6ac59e02fbe328f7476eaeb9c23de9b9288c2c2f39c"

DEPENDS += "libwpe"
RCONFLICTS_${PN} = "libwpe (< 1.8) wpebackend-fdo (< 1.8)"

LIC_FILES_CHKSUM = "file://Source/WebCore/LICENSE-LGPL-2.1;md5=a778a33ef338abbaf8b8a7c36b6eec80 "

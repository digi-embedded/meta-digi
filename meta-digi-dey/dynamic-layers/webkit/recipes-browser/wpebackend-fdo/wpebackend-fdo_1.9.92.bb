require recipes-browser/wpebackend-fdo/wpebackend-fdo.inc
inherit meson

SRC_URI = "https://wpewebkit.org/releases/${BPN}-${PV}.tar.xz"
SRC_URI[md5sum] = "5c2f7fab6623e0964bc0d1b4a01719f0"
SRC_URI[sha1sum] = "d07fdfec0df53c57e3ed36a1efffecf30d1b418c"
SRC_URI[sha256sum] = "fc5b388a91d6f2c22803e1a21a6759a314b4539e5169c6e272bfc953a05fbb85"

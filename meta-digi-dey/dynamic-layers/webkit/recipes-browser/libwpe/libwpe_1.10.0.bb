require recipes-browser/libwpe/libwpe.inc
require conf/include/devupstream.inc

SRC_URI = "https://wpewebkit.org/releases//${BPN}-${PV}.tar.xz"
SRC_URI[sha256sum] = "2415e270d45e3595ed4052bc105f733744dc2d3677e12ff4a831e5029841084d"

SRC_URI_class-devupstream = "git://github.com/WebPlatformForEmbedded/libwpe.git;protocol=https;branch=libwpe-1.10"
SRCREV_class-devupstream = "55877263583716303a893945418ec23cffdfcbbf"

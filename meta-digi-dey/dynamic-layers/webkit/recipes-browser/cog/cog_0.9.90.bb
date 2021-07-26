require recipes-browser/cog/cog.inc
require conf/include/devupstream.inc

SRC_URI = "https://wpewebkit.org/releases/${P}.tar.xz"
SRC_URI[sha256sum] = "933adc74e7b2b7f879a0159b073aa601d58865621891c443d1c2481f9eee6c97"

SRC_URI_class-devupstream = "git://github.com/Igalia/cog.git;protocol=https;branch=cog-0.10"
SRCREV_class-devupstream = "1e422e5055f72e9914341ce9535aaf375b821946"

DEPENDS += "wpewebkit (>= 2.30) wpebackend-fdo (>= 1.8)"

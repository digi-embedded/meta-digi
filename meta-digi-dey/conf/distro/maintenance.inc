# Use new NXP package mirror
FSL_MIRROR = "https://www.nxp.com/lgfiles/NMG/MAD/YOCTO/"

# Use GNU mirror instead of main server for GNU packages
GNU_MIRROR = "https://ftpmirror.gnu.org/gnu"

SRC_URI_prepend_pn-c-ares = "https://c-ares.org/archive/${BP}.tar.gz "
SRC_URI_remove_pn-c-ares = "http://c-ares.haxx.se/download/${BP}.tar.gz"
SRC_URI_prepend_pn-gdb = "${GNU_MIRROR}/gdb/gdb-${PV}.tar.xz "
SRC_URI_remove_pn-gdb = "http://ftp.gnu.org/gnu/gdb/gdb-${PV}.tar.xz"
SRC_URI_prepend_pn-libconfig = "https://sources.buildroot.net/${BPN}/${BP}.tar.gz "
SRC_URI_remove_pn-libconfig = "http://www.hyperrealm.com/${BPN}/${BP}.tar.gz"
SRC_URI_prepend_pn-lvm2 = "https://sourceware.org/pub/lvm2/old/LVM2.${PV}.tgz "
SRC_URI_remove_pn-lvm2 = "ftp://sources.redhat.com/pub/lvm2/old/LVM2.${PV}.tgz"
SRC_URI_prepend_pn-puzzles = "git://git.tartarus.org/simon/puzzles.git;branch=main "
SRC_URI_remove_pn-puzzles = "git://git.tartarus.org/simon/puzzles.git"
SRC_URI_prepend_pn-qtsmarthome = "https://artifacts.toradex.com/artifactory/tdxref-oe-prod-frankfurt/dunfell-5.x.y/release/4/sources/smarthome_src.tar.gz "
SRC_URI_remove_pn-qtsmarthome = "http://share.basyskom.com/demos/smarthome_src.tar.gz"
SRC_URI_prepend_pn-thin-provisioning-tools = "git://github.com/jthornber/thin-provisioning-tools;branch=main "
SRC_URI_remove_pn-thin-provisioning-tools = "git://github.com/jthornber/thin-provisioning-tools"
SRC_URI_prepend_pn-x264 = "git://github.com/mirror/x264.git;protocol=https;branch=stable "
SRC_URI_remove_pn-x264 = "git://git.videolan.org/x264.git"

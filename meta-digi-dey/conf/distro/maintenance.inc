# Use new NXP package mirror
FSL_MIRROR = "https://www.nxp.com/lgfiles/NMG/MAD/YOCTO/"

PV_pn-c-ares = "1.13.0+gitrAUTOINC+3be1924221"
SRC_URI_prepend_pn-c-ares = "git://github.com/c-ares/c-ares.git;branch=main "
SRC_URI_remove_pn-c-ares = "git://github.com/c-ares/c-ares.git"
SRC_URI_prepend_pn-libconfig = "https://sources.buildroot.net/${BPN}/${BP}.tar.gz "
SRC_URI_remove_pn-libconfig = "http://www.hyperrealm.com/${BPN}/${BP}.tar.gz"
SRC_URI_prepend_pn-puzzles = "git://git.tartarus.org/simon/puzzles.git;branch=main "
SRC_URI_remove_pn-puzzles = "git://git.tartarus.org/simon/puzzles.git"
SRC_URI_prepend_pn-tesseract-lang = "git://github.com/tesseract-ocr/tessdata.git;branch=main;protocol=https "
SRC_URI_remove_pn-tesseract-lang = "git://github.com/tesseract-ocr/tessdata.git"
SRC_URI_prepend_pn-thin-provisioning-tools = "git://github.com/jthornber/thin-provisioning-tools;branch=main "
SRC_URI_remove_pn-thin-provisioning-tools = "git://github.com/jthornber/thin-provisioning-tools"

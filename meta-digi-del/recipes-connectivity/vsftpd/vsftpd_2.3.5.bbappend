FILESEXTRAPATHS_prepend := "${THISDIR}/files"

PR_append = "+${DISTRO}.r0"
DEPENDS += "openssl"

# Inhibit warning about files already stripped
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"

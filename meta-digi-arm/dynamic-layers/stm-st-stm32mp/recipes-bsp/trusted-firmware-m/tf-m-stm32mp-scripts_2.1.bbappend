#
# Copyright (C) 2026, Digi International Inc.
#

do_install() {
    # Install assemble and signing firmware script
    install -d ${D}${bindir}
    install -m 755 ${WORKDIR}/scripts/create_st_m33fw_binary.sh ${D}${bindir}
    install -m 755 ${WORKDIR}/scripts/st_m33td_firmware_signature.sh ${D}${bindir}
    # Update version
    sed 's/^SIGN_VERSION=.*$/SIGN_VERSION='"${TF_M_VERSION}"'/' -i ${D}${bindir}/st_m33td_firmware_signature.sh
    # Install default MCUBOOT keys
    install -d ${D}${datadir}/tf-m/keys
    install -m 0644 ${S}/bl2/ext/mcuboot/root-EC-P256.pem ${D}${datadir}/tf-m/keys/root-ec-p256.pem
    install -m 0644 ${S}/bl2/ext/mcuboot/root-EC-P256_1.pem ${D}${datadir}/tf-m/keys/root-ec-p256_1.pem
    # Install all python scripts needed for assemble and sign
    install -d ${D}${datadir}/tf-m/scripts
    install -m 0755 ${S}/bl2/ext/mcuboot/scripts/assemble.py ${D}${datadir}/tf-m/scripts
    install -m 0755 ${S}/bl2/ext/mcuboot/scripts/macro_parser.py ${D}${datadir}/tf-m/scripts
    install -d ${D}${datadir}/tf-m/scripts/wrapper
    install -m 0755 ${S}/bl2/ext/mcuboot/scripts/wrapper/wrapper.py ${D}${datadir}/tf-m/scripts/wrapper
    # Install imgtool suite
    install -m 0755 ${WORKDIR}/${TF_M_EXTERNAL_SOURCES_ROOTDIR}/${TF_M_PATH_MCUBOOT}/scripts/imgtool.py ${D}${datadir}/tf-m/scripts/wrapper
    install -d ${D}${datadir}/tf-m/scripts/wrapper/imgtool
    cp -rf ${WORKDIR}/${TF_M_EXTERNAL_SOURCES_ROOTDIR}/${TF_M_PATH_MCUBOOT}/scripts/imgtool/* ${D}${datadir}/tf-m/scripts/wrapper/imgtool/
}


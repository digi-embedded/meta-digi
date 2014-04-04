# Copyright (C) 2012-2013 Freescale Semiconductor

DESCRIPTION = "Extra files for fsl-gui-image"
LICENSE = "LGPLv2"
PR = "r6"
S="${WORKDIR}"
LIC_FILES_CHKSUM = "file://LICENSE;md5=dfb0b24e10ca72d739b14d769d91cf9b"

SRC_URI += "file://qtbrowser.desktop \
            file://webkit.png \
            file://qtmediaplayer.desktop \
            file://qtmediaplayer.png \
            file://qtdemo.desktop \
            file://qtdemo.png \
            file://qthellogles2.desktop \
            file://hellogl_es2.png \
            file://LICENSE"

inherit allarch

do_install () {
    install -d ${D}/${datadir}/pixmaps
    install -d ${D}/${datadir}/applications
    install -m 0644 ${WORKDIR}/webkit.png ${D}/${datadir}/pixmaps
    install -m 0644 ${WORKDIR}/qtbrowser.desktop ${D}/${datadir}/applications
    install -m 0644 ${WORKDIR}/qtmediaplayer.png ${D}/${datadir}/pixmaps
    install -m 0644 ${WORKDIR}/qtmediaplayer.desktop ${D}/${datadir}/applications
    install -m 0644 ${WORKDIR}/qtdemo.png ${D}/${datadir}/pixmaps
    install -m 0644 ${WORKDIR}/qtdemo.desktop ${D}/${datadir}/applications
    install -m 0644 ${WORKDIR}/hellogl_es2.png ${D}/${datadir}/pixmaps
    install -m 0644 ${WORKDIR}/qthellogles2.desktop ${D}/${datadir}/applications
}

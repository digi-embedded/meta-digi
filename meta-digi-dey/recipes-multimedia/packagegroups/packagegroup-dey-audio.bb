#
# Copyright (C) 2012-2022 Digi International.
#
SUMMARY = "Audio packagegroup for DEY image"

PACKAGE_ARCH = "${MACHINE_ARCH}"
inherit packagegroup

ALSA_UTILS_PKGS = " \
    alsa-utils-alsactl \
    alsa-utils-alsamixer \
    alsa-utils-amixer \
    alsa-utils-aplay \
    alsa-utils-speakertest \
"

RDEPENDS:${PN} = "\
    libasound \
    alsa-state \
    alsa-states \
    ${ALSA_UTILS_PKGS} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'pulseaudio', 'pulseaudio-server pulseaudio-misc', '', d)} \
"

RDEPENDS:${PN}:append:ccimx6 = " card-detect"

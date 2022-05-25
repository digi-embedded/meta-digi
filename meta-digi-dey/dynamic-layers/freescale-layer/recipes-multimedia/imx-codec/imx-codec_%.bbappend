# Copyright (C) 2017, 2018 Digi International Inc.

# Empirically detected binaries that are not needed for a given platform
REDUNDANT_BINS ?= ""
REDUNDANT_BINS:ccimx6ul ?= " \
    usr/lib/imx-mm/audio-codec/wrap/lib_aacd_wrap_arm11_elinux.so* \
    usr/lib/imx-mm/audio-codec/wrap/lib_aacd_wrap_arm9_elinux.so* \
    usr/lib/imx-mm/audio-codec/wrap/lib_mp3d_wrap_arm11_elinux.so* \
    usr/lib/imx-mm/audio-codec/wrap/lib_mp3d_wrap_arm9_elinux.so* \
    usr/lib/imx-mm/audio-codec/wrap/lib_nbamrd_wrap_arm9_elinux.so* \
    usr/lib/imx-mm/audio-codec/wrap/lib_vorbisd_wrap_arm11_elinux.so* \
    usr/lib/imx-mm/audio-codec/wrap/lib_wbamrd_wrap_arm9_elinux.so* \
    usr/lib/lib_aac_dec_arm11_elinux.so* \
    usr/lib/lib_aac_dec_arm9_elinux.so* \
    usr/lib/lib_mp3_dec_arm11_elinux.so* \
    usr/lib/lib_mp3_dec_arm9_elinux.so* \
    usr/lib/lib_mp3_enc_arm11_elinux.so* \
    usr/lib/lib_mp3_enc_arm9_elinux.so* \
    usr/share/imx-mm/audio-codec/examples/aac-dec/bin/test_aac_dec_arm11_elinux* \
    usr/share/imx-mm/audio-codec/examples/aac-dec/bin/test_aac_dec_arm9_elinux* \
    usr/share/imx-mm/audio-codec/examples/mp3-dec/bin/test_mp3_dec_arm11_elinux* \
    usr/share/imx-mm/audio-codec/examples/mp3-dec/bin/test_mp3_dec_arm9_elinux* \
    usr/share/imx-mm/audio-codec/examples/mp3-enc/bin/test_mp3_enc_arm11_elinux* \
    usr/share/imx-mm/audio-codec/examples/mp3-enc/bin/test_mp3_enc_arm9_elinux* \
"

do_install:append() {
	for i in ${REDUNDANT_BINS}; do
		rm -f ${D}/${i}
	done
}

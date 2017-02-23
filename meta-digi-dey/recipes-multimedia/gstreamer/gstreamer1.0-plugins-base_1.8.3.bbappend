# Copyright 2015-2017, Digi International Inc.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BP}:"

SRC_URI_append = " \
    file://0001-basetextoverlay-make-memory-copy-when-video-buffer-s.patch \
    file://0002-gstplaysink-don-t-set-async-of-custom-text-sink-to-f.patch \
    file://0003-taglist-not-send-to-down-stream-if-all-the-frame-cor.patch \
    file://0004-handle-audio-video-decoder-error.patch \
    file://0005-gstaudiobasesink-print-warning-istead-of-return-ERRO.patch \
    file://0006-Disable-orc-optimization-for-lib-video-in-plugins-ba.patch \
"

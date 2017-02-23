# Copyright 2015-2017, Digi International Inc.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BP}:"

SRC_URI_append = " \
    file://0001-handle-base-parse-error.patch \
    file://0002-Fix-crash-with-gst-inspect-Chris-Lord-chris-openedha.patch \
    file://0003-unset-FLAG_DISCONT-when-push-to-adapter.patch \
    file://0004-Need-push-adapter-remainning-data-in-pass-through-mo.patch \
    file://0005-inputselector-should-proceed-non-active-pad-buffer-e.patch \
"

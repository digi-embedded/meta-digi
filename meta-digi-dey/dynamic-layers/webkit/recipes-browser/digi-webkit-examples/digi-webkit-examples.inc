# Copyright (C) 2020 Digi International.

# By default, use the Busybox HTTP daemon as the webserver for the examples.
WEBSERVER_PACKAGE ?= "busybox-httpd"
WEBSERVER_ROOT ?= "srv/www"

# List of samples we want accesible via the landing page
WEBGL_SAMPLES = " \
    aquarium \
    blob \
    dynamic-cubemap \
    electricflower \
    field \
    multiple-views \
    spacerocks \
    toon-shading \
"
# Folders containing elements required by the samples we've selected
WEBGL_SAMPLE_DEPS = " \
    colorpicker \
    css \
    fishtank \
    fonts \
    gradient-editor \
    images \
    jquery-ui-1.8.2.custom \
    js \
    lots-o-objects \
    shared \
    tdl \
"
# List of all folders that need to be installed
WEBGL_REQUIRED = " \
    ${WEBGL_SAMPLES} \
    ${WEBGL_SAMPLE_DEPS} \
"

# List of video sample formats
VIDEO_FORMATS = " \
    mov \
    webm \
"
# Name of the video sample
VIDEO_NAME = "big_buck_bunny"
VIDEO_NAME_UPPERCASE = "Big Buck Bunny"

# All packages involved in the webkit examples install their files in the
# webserver directory
FILES_${PN} = "/${WEBSERVER_ROOT}/*"

# Don't generate dbg or dev packages
PACKAGES = "${PN}"

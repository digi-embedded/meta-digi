# Copyright (C) 2020-2023, Digi International Inc.

SUMMARY = "A collection of WebGL samples"
DESCRIPTION = "This repo contains several examples of the WebGL JavaScript API, which allows web browsers to render 2D and 3D graphics with direct access to the system's GPU."
HOMEPAGE = "https://webglsamples.org/"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://LICENSE.md;md5=e20489d49c9f8517c8f8f3317844e7da"

WEBGL_SAMPLES_SRC ?= "git://github.com/WebGLSamples/WebGLSamples.github.io.git;protocol=https"
SRCBRANCH = "master"

SRC_URI = "${WEBGL_SAMPLES_SRC};branch=${SRCBRANCH}"
SRCREV = "dc4428bdc6ef2177f71d9e7bab164c43f9e29302"

S = "${WORKDIR}/git"

WEBSERVER_ROOT = "srv/www"

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

# The package contains static webpages, no need to configure or compile
do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install() {
	install -d ${D}/${WEBSERVER_ROOT}

	cd ${S}

	# All we need to do is copy the folders we want to the webserver root
	# path. Make sure not to copy any source assets, since they're quite
	# heavy.
	for sample in ${WEBGL_REQUIRED}; do
		find ${sample} -path *source_assets* -prune -false -o -type f \
		     -exec install -Dm 644 "{}" "${D}/${WEBSERVER_ROOT}/{}" \;
	done
	# Disable the aquarium options by default
	sed -i 's/enabled: true/enabled: false/g' ${D}/${WEBSERVER_ROOT}/aquarium/aquarium-common.js
}

# All packages involved in the webkit examples install their files in the
# webserver directory
FILES:${PN} = "/${WEBSERVER_ROOT}/*"

# Don't generate dbg or dev packages
PACKAGES = "${PN}"

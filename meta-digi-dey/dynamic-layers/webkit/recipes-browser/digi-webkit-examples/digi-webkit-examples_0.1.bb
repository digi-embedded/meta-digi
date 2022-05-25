# Copyright (C) 2020 Digi International.

SUMMARY = "A webpage containing several examples for the WPE WebKit on Digi embedded devices"
DESCRIPTION = "This webpage provides examples that show how the WPE WebKit leverages the hardware capabilities of Digi embedded devices for improved performance"
LICENSE = "MPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MPL-2.0;md5=815ca599c9df247a0c7f619bab123dad"

SRC_URI = "${DIGI_PKG_SRC}/${BPN}-${PV}.tar.gz"

SRC_URI[md5sum] = "cc4b81cf92135be3e231375e9a22fe6a"
SRC_URI[sha256sum] = "26ed0fafcf9d66eabac4c6963ea2e3fb46d3cc94d76d50413883f28f9c28f737"

S = "${WORKDIR}/${PN}-${PV}"

require digi-webkit-examples.inc

RDEPENDS:${PN} = " \
    cog \
    video-examples \
    webglsamples \
    ${WEBSERVER_PACKAGE} \
"

# The package contains static webpages, no need to configure or compile
do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install() {
	install -d ${D}/${WEBSERVER_ROOT}/style
	install -d ${D}/${WEBSERVER_ROOT}/images
	install -m 644 ${S}/examples_viewer.html ${D}/${WEBSERVER_ROOT}
	install -m 644 ${S}/index.html ${D}/${WEBSERVER_ROOT}
	install -m 644 ${S}/style/* ${D}/${WEBSERVER_ROOT}/style
	install -m 644 ${S}/images/* ${D}/${WEBSERVER_ROOT}/images

	# Most entry points for the WebGL samples have the same format:
	# <name>/<name>.html. Since we might define different sample lists per
	# platform, we should generate the list of samples dynamically.
	SAMPLE_LIST=""
	for sample in ${WEBGL_SAMPLES}; do
		SAMPLE_LIST="${SAMPLE_LIST}\n$(sed s/_name_/${sample}/g ${S}/webgl_demo_template)"
	done

	sed -i s,##WEBGL_SAMPLE_LIST##,"${SAMPLE_LIST}",g ${D}/${WEBSERVER_ROOT}/index.html

	# Use the same method to dynamically generate the list of video
	# examples.
	SAMPLE_LIST=""
	for format in ${VIDEO_FORMATS}; do
		SAMPLE_LIST="${SAMPLE_LIST}\n$(sed s/_fmt_/${format}/g ${S}/video_demo_template | \
		                               sed s/_name_/${VIDEO_NAME}/g | \
		                               sed s/_name-upper_/"${VIDEO_NAME_UPPERCASE}"/g)"
	done

	sed -i s,##VIDEO_SAMPLE_LIST##,"${SAMPLE_LIST}",g ${D}/${WEBSERVER_ROOT}/index.html
}

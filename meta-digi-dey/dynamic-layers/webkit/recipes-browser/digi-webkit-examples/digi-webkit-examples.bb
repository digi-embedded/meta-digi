# Copyright (C) 2020 Digi International.

SUMMARY = "A webpage containing several examples for the WPE WebKit on Digi embedded devices"
DESCRIPTION = "This webpage provides examples that show how the WPE WebKit leverages the hardware capabilities of Digi embedded devices for improved performance"
LICENSE = "MPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MPL-2.0;md5=815ca599c9df247a0c7f619bab123dad"

SRC_URI = " \
    file://index.html \
    file://digi.css \
"

S = "${WORKDIR}"

require digi-webkit-examples.inc

RDEPENDS_${PN} = " \
    cog \
    video-examples \
    webglsamples \
    ${WEBSERVER_PACKAGE} \
"

VPU_NOTE = "This means that, if the video format is supported by the VPU, WebKit will use the VPU to decode the video."
VPU_NOTE_ccimx8mn = "Since the ConnectCore 8M Nano doesn't have a VPU, WebKit will decode the videos using gstreamer software."

# The package contains static webpages, no need to configure or compile
do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install() {
	install -d ${D}/${WEBSERVER_ROOT}
	install -m 644 ${S}/index.html ${D}/${WEBSERVER_ROOT}
	install -m 644 ${S}/digi.css ${D}/${WEBSERVER_ROOT}

	# Most entry points for the WebGL samples have the same format:
	# <name>/<name>.html. Since we might define different sample lists per
	# platform, we should generate the list of samples dynamically.
	SAMPLE_LIST=""
	ENTRY='<li><p><a href="_name_/_name_.html">_name_</a></p></li>'
	for sample in ${WEBGL_SAMPLES}; do
		SAMPLE_LIST="${SAMPLE_LIST}\n$(echo ${ENTRY} | sed s/_name_/${sample}/g)"
	done

	SAMPLE_LIST="${SAMPLE_LIST}\n"

	sed -i s,##WEBGL_SAMPLE_LIST##,"${SAMPLE_LIST}",g ${D}/${WEBSERVER_ROOT}/index.html

	# Add a note regarding the video decoding process, which depends on the
	# platform.
	sed -i s/##VPU_NOTE##/"${VPU_NOTE}"/g ${D}/${WEBSERVER_ROOT}/index.html

	# Use the same method to dynamically generate the list of video
	# examples.
	SAMPLE_LIST=""
	ENTRY='<li><p><a href="videos/_name_\">_name_</a></p></li>'
	for sample in ${VIDEO_SAMPLES}; do
		SAMPLE_LIST="${SAMPLE_LIST}\n$(echo ${ENTRY} | sed s/_name_/${sample}/g)"
	done

	SAMPLE_LIST="${SAMPLE_LIST}\n"

	sed -i s,##VIDEO_SAMPLE_LIST##,"${SAMPLE_LIST}",g ${D}/${WEBSERVER_ROOT}/index.html
}

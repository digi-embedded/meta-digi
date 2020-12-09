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
    webglsamples \
    ${WEBSERVER_PACKAGE} \
"

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
}

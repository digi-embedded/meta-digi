SUMMARY = "DMCIPP ISP control tools"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://dcmipp-isp-ctrl/COPYING;md5=f8001cce2bab8ab39ddcb12684e4bdf4"

SRC_URI = "git://github.com/STMicroelectronics/st-openstlinux-application.git;protocol=https;branch=main"

# Modify these as desired
PV = "5.1+git${SRCPV}"
SRCREV = "a7f953caba8823e601eb85c5e6052b1e81188cd7"

S = "${WORKDIR}/git"

do_compile () {
	cd ${S}/dcmipp-isp-ctrl
	oe_runmake
}

do_install () {
	install -d ${D}${prefix}/local/demo/bin
	install -m 0755 ${B}/dcmipp-isp-ctrl/dcmipp-isp-ctrl ${D}${prefix}/local/demo/bin/
}
FILES:${PN} += "${prefix}/local/demo/bin/dcmipp-isp-ctrl"


# Copyright 2017-2018 NXP
# Released under the MIT license (see COPYING.MIT for the terms)

SUMMARY = "Installs i.MX-specific kernel headers"
DESCRIPTION = "Installs i.MX-specific kernel headers to userspace. \
New headers are installed in ${includedir}/imx."
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=d7810fab7487fb0aad327b76f1be7cd7"

require recipes-kernel/linux/linux-dey-src.inc

S = "${WORKDIR}/git"

do_compile[noexec] = "1"

IMX_UAPI_HEADERS = " \
    dma-buf.h \
    hantrodec.h \
    hx280enc.h \
    ion.h \
    ipu.h \
    isl29023.h \
    mxc_asrc.h \
    mxc_dcic.h \
    mxc_mlb.h \
    mxc_sim_interface.h \
    mxc_v4l2.h \
    mxcfb.h \
    pxp_device.h \
    pxp_dma.h \
    videodev2.h \
"

do_install() {
	# We install all headers inside of B so we can copy only the
	# whitelisted ones, and there is no risk of a new header to be
	# installed by mistake.
	oe_runmake headers_install INSTALL_HDR_PATH=${B}${exec_prefix}

	# FIXME: The ion.h is still on staging so "promote" it for now
	cp ${S}/drivers/staging/android/uapi/ion.h ${B}${includedir}/linux

	# Install whitelisted headers only
	for h in ${IMX_UAPI_HEADERS}; do
		install -D -m 0644 ${B}${includedir}/linux/$h \
				${D}${includedir}/imx/linux/$h
	done
}

DEPENDS += "rsync-native"

ALLOW_EMPTY_${PN} = "1"

PACKAGE_ARCH = "${MACHINE_SOCARCH}"

COMPATIBLE_MACHINE = "(ccimx6ul|ccimx8x|ccimx8m|ccimx6)"

# Copyright 2017-2020 NXP
# Released under the MIT license (see COPYING.MIT for the terms)

SUMMARY = "Installs i.MX-specific kernel headers"
DESCRIPTION = "Installs i.MX-specific kernel headers to userspace. \
New headers are installed in ${includedir}/imx."
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=bbea815ee2795b2f4230826c0c6b8814"

SRCBRANCH = "v5.4.70/master"
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

# Allow to build empty main package, this is required in order for -dev package
# to be propagated into the SDK
#
# Without this setting the RDEPENDS in other recipes fails to find this
# package, therefore causing the -dev package also to be skipped effectively not
# populating it into SDK
ALLOW_EMPTY:${PN} = "1"

INHIBIT_DEFAULT_DEPS = "1"
DEPENDS += "unifdef-native bison-native rsync-native"

PACKAGE_ARCH = "${MACHINE_SOCARCH}"

COMPATIBLE_MACHINE = "(ccimx6ul|ccimx8x|ccimx8m|ccimx6)"

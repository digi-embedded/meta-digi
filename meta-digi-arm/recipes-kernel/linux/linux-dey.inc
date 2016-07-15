# Copyright (C) 2013 Digi International

SUMMARY = "Linux kernel for Digi boards"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=d7810fab7487fb0aad327b76f1be7cd7"

DEPENDS += "lzop-native bc-native"

inherit kernel fsl-kernel-localversion

LOCALVERSION = "-dey"

# Select internal or Github Linux repo
LINUX_GIT_URI = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${DIGI_GIT}linux-2.6.git', '${DIGI_GITHUB_GIT}/linux.git', d)}"

SRC_URI = " \
    ${LINUX_GIT_URI};nobranch=1 \
    file://defconfig \
"

S = "${WORKDIR}/git"

# We need to pass it as param since kernel might support more than one
# machine, with different entry points
KERNEL_EXTRA_ARGS += "LOADADDR=${UBOOT_ENTRYPOINT}"

do_deploy_append() {
	(cd ${DEPLOYDIR} && ln -sf ${KERNEL_IMAGE_BASE_NAME}.bin ${KERNEL_IMAGE_SYMLINK_NAME})
}

FILES_kernel-image += "/boot/config-${KERNEL_VERSION}"

# Don't include kernels in standard images
RDEPENDS_kernel-base = ""

# Copyright (C) 2013 Digi International

require recipes-kernel/linux/linux-imx.inc

LOCALVERSION = "-dey"

SRC_URI_external = "${DIGI_GITHUB_GIT}/yocto-linux.git;protocol=git;nobranch=1"
SRC_URI_internal = "${DIGI_GIT}linux-2.6.git;protocol=git;nobranch=1"
SRC_URI = " \
    ${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${SRC_URI_internal}', '${SRC_URI_external}', d)} \
    file://defconfig \
    ${KERNEL_CFG_FRAGS} \
"

do_configure() {
	for i in $(echo ${WORKDIR}/*.cfg); do
		[ "${i}" = "${WORKDIR}/*.cfg" ] && continue
		cat ${i} >> '${S}/.config'
	done
	kernel_do_configure
}

do_deploy_append() {
	(cd ${DEPLOYDIR} && ln -sf ${KERNEL_IMAGE_BASE_NAME}.bin ${KERNEL_IMAGE_SYMLINK_NAME})
}

FILES_kernel-image += "/boot/config-${KERNEL_VERSION}"

# Don't include kernels in standard images
RDEPENDS_kernel-base = ""

# Copyright (C) 2018-2022 Digi International

LOCALVERSION = "-dey"

# Select internal or Github Linux repo
LINUX_URI_STASH = "${DIGI_MTK_GIT}/linux/linux.git;protocol=ssh"
LINUX_URI_GITHUB = "${DIGI_GITHUB_GIT}/linux.git;protocol=https"
LINUX_GIT_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1' , '${LINUX_URI_STASH}', '${LINUX_URI_GITHUB}', d)}"

SRC_URI = "${LINUX_GIT_URI};branch=${SRCBRANCH}"

# A user can provide his own kernel 'defconfig' file by:
# - setting the variable KERNEL_DEFCONFIG to a custom kernel configuration file
#   inside the kernel repository.
# - setting the variable KERNEL_DEFCONFIG to a kernel configuration file using
#   the full path to the file.
# - clearing the variable KERNEL_DEFCONFIG and providing a kernel configuration
#   file in the layer (in this case the file must be named 'defconfig').
# Otherwise the default platform's kernel configuration file will be taken from
# the Linux source code tree.
do_copy_defconfig[vardeps] += "KERNEL_DEFCONFIG"
do_copy_defconfig[dirs] = "${S}"
do_copy_defconfig () {
        if [ -n "${KERNEL_DEFCONFIG}" ]; then
                cp -f ${KERNEL_DEFCONFIG} ${WORKDIR}/defconfig
        fi
}
addtask copy_defconfig after do_patch before do_kernel_localversion

S = "${WORKDIR}/git"

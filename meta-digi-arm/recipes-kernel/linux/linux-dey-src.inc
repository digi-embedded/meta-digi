# Copyright (C) 2018-2022 Digi International

LOCALVERSION = "-dey"
SRCREV = "5ed8aa9a3de7fe71d715b1faecba557cddd72c5d"

# Select internal or Github Linux repo
DIGI_LOG_REPO = "linux-2.6.git"
DIGI_MTK_REPO = "linux/linux.git"
GITHUB_REPO = "linux.git"
LINUX_GIT_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1' , \
					oe.utils.conditional('DIGI_GIT', '${DIGI_LOG_GIT}', '${DIGI_GIT}/${DIGI_LOG_REPO}', '${DIGI_GIT}/${DIGI_MTK_REPO};protocol=ssh', d), \
					'${DIGI_GITHUB_GIT}/${GITHUB_REPO};protocol=https', d)}"

SRC_URI = "${LINUX_GIT_URI};nobranch=1"
SRC_URI_stmp32mpcommon = "${LINUX_GIT_URI};nobranch=1"

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

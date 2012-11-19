#
# Copyright (C) 2012 Digi International.
#
DESCRIPTION = "DEL busybox based image (non graphical)."

IMAGE_INSTALL = "task-del-core ${ROOTFS_PKGMANAGE_BOOTSTRAP} ${CORE_IMAGE_EXTRA_INSTALL}"

IMAGE_LINGUAS = " "

LICENSE = "MIT"

inherit core-image
inherit del-image

IMAGE_FEATURES = "core-ssh-dropbear"
IMAGE_FEATURES += "del-audio"

# core-image disables the root password if debug-tweak is not enabled.
# This override will use the shadow file instead.
zap_root_password () {
        sed 's%^root:[^:]*:%root:x:%' < ${IMAGE_ROOTFS}/etc/passwd >${IMAGE_ROOTFS}/etc/passwd.new
        mv ${IMAGE_ROOTFS}/etc/passwd.new ${IMAGE_ROOTFS}/etc/passwd
}

IMAGE_ROOTFS_SIZE = "8192"

# remove not needed ipkg informations
ROOTFS_POSTPROCESS_COMMAND += "remove_packaging_data_files ; "

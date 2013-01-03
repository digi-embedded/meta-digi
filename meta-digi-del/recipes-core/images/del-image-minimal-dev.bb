#
# Copyright (C) 2012 Digi International.
#
DESCRIPTION = "DEL busybox based image (non graphical) suitable for development work."

IMAGE_INSTALL = "task-del-core ${ROOTFS_PKGMANAGE} ${CORE_IMAGE_EXTRA_INSTALL}"

IMAGE_LINGUAS = " "

LICENSE = "MIT"

inherit core-image
inherit del-image

IMAGE_FEATURES += "core-ssh-dropbear"
IMAGE_FEATURES += "dev-pkgs"

# Adding debug-tweaks will enable empty password login.
# Note that adding debug-tweaks to EXTRA_IMAGE_FEATURES will not
# allow for empty password logins.
IMAGE_FEATURES += "debug-tweaks"

# remove not needed ipkg informations
ROOTFS_POSTPROCESS_COMMAND += "remove_packaging_data_files; del_rootfs_tuning;"

#
# Copyright (C) 2013 Digi International.
#

require dey-image-graphical.bb

DESCRIPTION = "Image that includes everything within dey-image-grahical plus meta-toolchain, \
development headers and libraries to form a standalone SDK."

# Dropbear clashes with openssh which is included by tools-debug.
IMAGE_FEATURES := "${@oe_filter_out('ssh-server-dropbear', bb.data.getVar('IMAGE_FEATURES', d, 1), d)}"
IMAGE_FEATURES += "dev-pkgs tools-sdk\
	tools-debug eclipse-debug tools-profile tools-testapps debug-tweaks"

IMAGE_INSTALL += "kernel-dev"

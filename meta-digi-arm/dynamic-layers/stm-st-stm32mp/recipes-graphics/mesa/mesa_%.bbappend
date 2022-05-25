# Copyright (C) 2022 Digi International.

#
# The bbappend in 'meta-st-stm32mp' layer misconfigured the PACKAGECONFIG
# options of the mesa package, leading to a build failure. This bbappends
# overrides the build configuration.
#
# @TODO: remove once the bbappend in 'meta-st-stm32mp' is fixed.
#
PACKAGECONFIG:ccmp15 = " \
	gallium \
	${@bb.utils.filter('DISTRO_FEATURES', 'x11 vulkan wayland', d)} \
	${@bb.utils.contains('DISTRO_FEATURES', 'opengl', 'opengl egl gles gbm virgl', '', d)} \
	${@bb.utils.contains('DISTRO_FEATURES', 'x11 opengl', 'dri3', '', d)} \
	${@bb.utils.contains('DISTRO_FEATURES', 'x11 vulkan', 'dri3', '', d)} \
	${@bb.utils.contains('TCLIBC', 'glibc', 'elf-tls', '', d)} \
"

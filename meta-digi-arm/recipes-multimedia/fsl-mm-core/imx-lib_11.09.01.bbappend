PR_append = "+digi.0"

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}"
SRC_URI += " file://imx-lib-11.09.01-0003-vpu-do-not-error-if-no-VPU-IRAM-present.patch "

# We need to hardcode the PLATFORM variable per-machine because the one defined
# in FSL layer (PLATFORM_mx5) is not passed to do_compile throwing an error:
# "Unspecified PLATFORM variable". This is probably a bug in yocto build system
# maybe fixed in newer versions.
PLATFORM_ccxmx51js = "IMX51"
PLATFORM_ccxmx53js = "IMX51"

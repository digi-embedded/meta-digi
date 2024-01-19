SUMMARY = "LVGL Demo Application"
HOMEPAGE = "https://github.com/digi-embedded/lv_port_linux_frame_buffer"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=802d3d83ae80ef5f343050bf96cce3a4 \
                    file://lv_drivers/LICENSE;md5=d6fc0df890c5270ef045981b516bb8f2 \
                    file://lvgl/LICENCE.txt;md5=bf1198c89ae87f043108cea62460b03a"

SRCBRANCH ?= "dey/master"

SRC_URI = " \
    gitsm://github.com/digi-embedded/lv_port_linux_frame_buffer.git;branch=${SRCBRANCH};protocol=https \
"
SRCREV = "0a799d22a5aaf9de18aca428579945a0a9c2c270"

EXTRA_OEMAKE = "DESTDIR=${D}"

# By default, use wayland backend if possible.
# If unavailable, fall back to a secondary backend
MINIMAL_BACKEND ?= "fbdev"
MINIMAL_BACKEND:imxdrm = "drm"
MINIMAL_BACKEND:ccmp15 = "sdl"
PACKAGECONFIG = "${@bb.utils.contains('DISTRO_FEATURES', 'wayland', 'wayland', '${MINIMAL_BACKEND}', d)}"

require lv-drivers.inc

inherit cmake

S = "${WORKDIR}/git"

TARGET_CFLAGS += "-I${STAGING_INCDIR}/libdrm"

# Change DRM card used for i.MX8-based platforms
LVGL_CONFIG_DRM_CARD:mx8-generic-bsp = "/dev/dri/card1"

LVGL_CONFIG_HOR_RES ?= "800"
LVGL_CONFIG_VER_RES ?= "480"

do_configure:prepend() {
	if [ "${LVGL_CONFIG_USE_DRM}" -eq 1 ] ; then
		# Add libdrm build dependency
		sed -i '/^target_link_libraries/ s@lvgl::drivers@& drm@' "${S}/CMakeLists.txt"
	fi

	if [ "${LVGL_CONFIG_USE_SDL}" -eq 1 ] ; then
		# Add libsdl build dependency
		sed -i '/^target_link_libraries/ s@lvgl::drivers@& SDL2@' "${S}/CMakeLists.txt"
	fi

	if [ "${LVGL_CONFIG_USE_WAYLAND}" -eq 1 ] ; then
		# Add wayland build dependencies
		sed -i '/^target_link_libraries/ s@lvgl::drivers@& wayland-client wayland-cursor xkbcommon@' "${S}/CMakeLists.txt"
	fi

	# Configure the app's dimensions
	sed -e "s|\(^#define *LV_DRV_DISP_HOR_RES *\).*|\1${LVGL_CONFIG_HOR_RES}|g" \
	    -e "s|\(^#define *LV_DRV_DISP_VER_RES *\).*|\1${LVGL_CONFIG_VER_RES}|g" \
	    \
	    -i "${S}/lv_drv_conf.h"
}

do_install:append() {
	install -d ${D}${bindir}
	install -m 0755 ${B}/lvgl_fb ${D}${bindir}/lvgl_demo
}

COMPATIBLE_MACHINE = "(ccimx6$|ccimx6ul|ccimx8m|ccimx8x|ccimx93|ccmp15)"

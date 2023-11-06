SUMMARY = "LVGL Demo Application"
HOMEPAGE = "https://github.com/lvgl/lv_port_linux_frame_buffer"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=802d3d83ae80ef5f343050bf96cce3a4 \
                    file://lv_drivers/LICENSE;md5=d6fc0df890c5270ef045981b516bb8f2 \
                    file://lvgl/LICENCE.txt;md5=bf1198c89ae87f043108cea62460b03a"

SRC_URI = " \
    gitsm://github.com/lvgl/lv_port_linux_frame_buffer.git;branch=master;protocol=https \
    file://0001-Make-demo-compatible-with-any-backend.patch \
    file://0002-Miscellaneous-improvements.patch \
"
SRCREV = "adf2c4490e17a1b9ec1902cc412a24b3b8235c8e"

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
}

do_install:append() {
	install -d ${D}${bindir}
	install -m 0755 ${B}/lvgl_fb ${D}${bindir}/lvgl_demo
}

COMPATIBLE_MACHINE = "(ccimx6$|ccimx6ul|ccimx8m|ccimx8x|ccimx93|ccmp15)"

SUMMARY = "LVGL Demo Application for Framebuffer"
HOMEPAGE = "https://github.com/lvgl/lv_port_linux_frame_buffer"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=802d3d83ae80ef5f343050bf96cce3a4 \
                    file://lvgl/LICENCE.txt;md5=4570b6241b4fced1d1d18eb691a0e083"

SRC_URI = "\
	git://github.com/lvgl/lv_port_linux_frame_buffer.git;protocol=https;branch=release/v9.3;name=demo \
	git://github.com/lvgl/lvgl;protocol=https;branch=release/v9.3;name=lvgl;subdir=git/lvgl \
	file://0001-lvgl-demo-remove-demo-slideshow.patch \
	file://0004-lvgl-demo-add-input-device-discovery-support-to-LVGL.patch \
	file://lvgl-demo-init \
	file://lvgl-demo-init.service \
"

SRC_URI:append:ccimx6ul = "\
	file://0003-CMakefile-remove-libdrm-dependency-when-building-fbd.patch \
"

SRC_URI:append:ccimx6 = "\
	file://0003-CMakefile-remove-libdrm-dependency-when-building-fbd.patch \
"

SRCREV_demo = "d07de027a8eb220f4e20f0e1b8be28729332e9ea"
SRCREV_lvgl = "c033a98afddd65aaafeebea625382a94020fe4a7"
SRCREV_FORMAT = "demo_lvgl"

EXTRA_OEMAKE = "DESTDIR=${D}"

LVGL_CONFIG_DRM_CARD ?= "/dev/dri/card0"
# Change DRM card used for i.MX8-based platforms
LVGL_CONFIG_DRM_CARD:mx8-generic-bsp = "/dev/dri/card1"
LVGL_CONFIG_FBDEV_DEVICE ?= "/dev/fb0"
# Change framebuffer used for the ccimx6/ccimx6qp (HDMI display)
LVGL_CONFIG_FBDEV_DEVICE:ccimx6 = "/dev/fb3"
LVGL_CONFIG_LV_USE_LOG    = "1"
LVGL_CONFIG_LV_LOG_PRINTF = "1"
LVGL_CONFIG_LV_MEM_SIZE = "(256 * 1024U)"
LVGL_CONFIG_LV_USE_FONT_COMPRESSED = "1"

require lv-conf.inc

inherit cmake systemd update-rc.d

S = "${WORKDIR}/git"

do_configure:prepend() {
	if [ "${LVGL_CONFIG_USE_SDL}" -eq 1 ] ; then
		# Add libsdl build dependency, SDL2_image has no cmake file
		sed -i '/^target_link_libraries/ s@pthread@& SDL2_image@' "${S}/CMakeLists.txt"
	fi
}

do_install:append() {
	install -d ${D}${bindir}
	install -m 0755 ${S}/bin/main ${D}${bindir}/lvgl-demo

	# Install systemd service
	if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
		# Install systemd unit files
		install -d ${D}${systemd_unitdir}/system
		install -m 0644 ${WORKDIR}/lvgl-demo-init.service ${D}${systemd_unitdir}/system/
	fi

	# Install wrapper bootscript to launch LVGL demo on boot
	install -d ${D}${sysconfdir}/init.d
	install -m 0755 ${WORKDIR}/lvgl-demo-init ${D}${sysconfdir}/lvgl-demo-init
	sed -i -e 's,##LVGL_CONFIG_DRM_CARD##,${LVGL_CONFIG_DRM_CARD},g' \
	    -i -e 's,##LVGL_CONFIG_FBDEV_DEVICE##,${LVGL_CONFIG_FBDEV_DEVICE},g' \
	    -i ${D}${sysconfdir}/lvgl-demo-init
	ln -sf ${sysconfdir}/lvgl-demo-init ${D}${sysconfdir}/init.d/lvgl-demo-init
}

PACKAGES =+ "${PN}-init"
FILES:${PN}-init = " \
    ${sysconfdir}/lvgl-demo-init \
    ${sysconfdir}/init.d/lvgl-demo-init \
    ${systemd_unitdir}/system/lvgl-demo-init.service \
"

INITSCRIPT_PACKAGES += "${PN}-init"
INITSCRIPT_NAME:${PN}-init = "lvgl-demo-init"
INITSCRIPT_PARAMS:${PN}-init = "start 99 3 5 . stop 20 0 1 2 6 ."

SYSTEMD_PACKAGES = "${PN}-init"
SYSTEMD_SERVICE:${PN}-init = "lvgl-demo-init.service"

COMPATIBLE_MACHINE = "(ccimx6$|ccimx6ul|ccimx8m|ccimx8x|ccimx93|ccimx95|ccmp15|ccmp2)"

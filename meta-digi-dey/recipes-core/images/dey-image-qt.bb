#
# Copyright (C) 2016-2023 Digi International.
#
require dey-image-graphical.inc

DESCRIPTION = "DEY image with QT graphical libraries"

GRAPHICAL_CORE = "qt"

add_cinematicexperience_shortcut() {
	if [ -f ${IMAGE_ROOTFS}${datadir}/icons/hicolor/24x24/icon_qt.png ] && [ -f ${IMAGE_ROOTFS}${sysconfdir}/xdg/weston/weston.ini ]; then
		printf "\n[launcher]\nicon=${datadir}/icons/hicolor/24x24/icon_qt.png\npath=${bindir}/cinematic-experience\n" >> ${IMAGE_ROOTFS}${sysconfdir}/xdg/weston/weston.ini
	fi
}
ROOTFS_POSTPROCESS_COMMAND:append:imxgpu = " add_cinematicexperience_shortcut;"
ROOTFS_POSTPROCESS_COMMAND:append:ccmp15 = " add_cinematicexperience_shortcut;"
ROOTFS_POSTPROCESS_COMMAND:append:ccimx93 = " add_cinematicexperience_shortcut;"

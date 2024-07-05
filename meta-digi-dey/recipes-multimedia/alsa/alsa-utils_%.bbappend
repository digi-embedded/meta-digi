# Copyright (C) 2024 Digi International.

do_install:append() {
	if ${@bb.utils.contains('DISTRO_FEATURES','systemd','true','false',d)}; then
		sed -i -e 's:alsactl restore:alsactl --no-ucm restore:g' \
			${D}${systemd_unitdir}/system/alsa-restore.service
	fi
}

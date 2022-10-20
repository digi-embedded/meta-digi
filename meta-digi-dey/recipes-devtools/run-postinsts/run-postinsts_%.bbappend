# Copyright (C) 2022 Digi International

do_install:append() {
	# Add ordering dependency between postinsts and ldconfig service
	sed -i -e '/After=/ s/$/ ldconfig.service/' \
               ${D}${systemd_system_unitdir}/run-postinsts.service
}

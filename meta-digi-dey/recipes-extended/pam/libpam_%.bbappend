# Copyright (C) 2025, Digi International Inc.

do_install:append:ccmp13() {
	sed -i '/pam_systemd.so/d' ${D}${sysconfdir}/pam.d/common-session
}

# Copyright (C) 2017 Digi International Inc.

do_install_append() {
	# NetworkManager manages the 'resolv.conf' file globally, so
	# remove the 'pppd' specific scripts for DNS.
	rm -f ${D}${sysconfdir}/ppp/ip-up.d/08setupdns
	rm -f ${D}${sysconfdir}/ppp/ip-down.d/92removedns
}

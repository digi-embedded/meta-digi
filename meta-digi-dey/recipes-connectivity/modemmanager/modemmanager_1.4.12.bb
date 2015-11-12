SUMMARY = "ModemManager is a daemon controlling broadband devices/connections"
DESCRIPTION = "ModemManager is a DBus-activated daemon which controls mobile broadband (2G/3G/4G) devices and connections"
HOMEPAGE = "http://www.freedesktop.org/wiki/Software/ModemManager/"
LICENSE = "GPLv2 & LGPLv2.1"
LIC_FILES_CHKSUM = " \
    file://COPYING;md5=b234ee4d69f5fce4486a80fdaf4a4263 \
    file://COPYING.LIB;md5=4fbd65380cdd255951079008b364516c \
"

inherit autotools gettext gtk-doc systemd

DEPENDS = "glib-2.0 libmbim libqmi udev dbus-glib"

SRC_URI = " \
	http://www.freedesktop.org/software/ModemManager/ModemManager-${PV}.tar.xz \
	file://cellularifupdown \
	file://0001-gobi-remove-plugin.patch \
"

SRC_URI[md5sum] = "66cc7266b15525cb366253e6639fc564"
SRC_URI[sha256sum] = "7ef5035375a953b285a742591df0a65fd442f4641ce4d8f4392a41d6d6bc70b3"

S = "${WORKDIR}/ModemManager-${PV}"

EXTRA_OECONF = "--with-polkit=none"

FILES_${PN} += " \
    ${datadir}/icons \
    ${datadir}/polkit-1 \
    ${libdir}/ModemManager \
    ${systemd_unitdir}/system \
"

FILES_${PN}-dev += " \
    ${datadir}/dbus-1 \
    ${libdir}/ModemManager/*.la \
"

FILES_${PN}-staticdev += " \
    ${libdir}/ModemManager/*.a \
"

FILES_${PN}-dbg += "${libdir}/ModemManager/.debug"

SYSTEMD_SERVICE_${PN} = "ModemManager.service"
# no need to start on boot - dbus will start on demand
SYSTEMD_AUTO_ENABLE = "disable"

do_install_append() {
	# Install ifupdown script for cellular interfaces
	install -d ${D}${sysconfdir}/network/if-pre-up.d/ ${D}${sysconfdir}/network/if-post-down.d/
	install -m 0755 ${WORKDIR}/cellularifupdown ${D}${sysconfdir}/network/if-pre-up.d/
	ln -sf ../if-pre-up.d/cellularifupdown ${D}${sysconfdir}/network/if-post-down.d/cellularifupdown
}

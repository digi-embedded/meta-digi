SUMMARY = "ModemManager is a daemon controlling broadband devices/connections"
DESCRIPTION = "ModemManager is a DBus-activated daemon which controls mobile \
broadband (2G/3G/4G) devices and connections"
HOMEPAGE = "http://www.freedesktop.org/wiki/Software/ModemManager/"
LICENSE = "GPL-2.0 & LGPL-2.1"
LIC_FILES_CHKSUM = " \
    file://COPYING;md5=b234ee4d69f5fce4486a80fdaf4a4263 \
    file://COPYING.LIB;md5=4fbd65380cdd255951079008b364516c \
"

DEPENDS = "glib-2.0 intltool-native libgudev"

PV = "1.7.0+git${SRCPV}"

SRC_URI = " \
    git://anongit.freedesktop.org/git/ModemManager/ModemManager.git;protocol=https \
    file://0001-configure.ac-add-foreign-automake-option.patch \
"
SRCREV = "d41d717112e6a183a0df510c210e80a86fc11060"

S = "${WORKDIR}/git"

PACKAGECONFIG ??= "mbim qmi polkit \
    ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'systemd', '', d)} \
"
PACKAGECONFIG[systemd] = "--with-systemdsystemunitdir=${systemd_unitdir}/system/,,"
PACKAGECONFIG[polkit] = "--with-polkit=yes,--with-polkit=no,polkit"
PACKAGECONFIG[mbim] = "--with-mbim,--enable-mbim=no,libmbim"
PACKAGECONFIG[qmi] = "--with-qmi,--without-qmi,libqmi"

inherit autotools bash-completion gettext gobject-introspection gtk-doc pkgconfig systemd vala

FILES_${PN} += " \
    ${datadir}/icons \
    ${datadir}/polkit-1 \
    ${datadir}/dbus-1 \
    ${libdir}/ModemManager \
    ${systemd_unitdir}/system \
"
FILES_${PN}-dev += "${libdir}/ModemManager/*.la"
FILES_${PN}-staticdev += "${libdir}/ModemManager/*.a"
FILES_${PN}-dbg += "${libdir}/ModemManager/.debug"

SYSTEMD_SERVICE_${PN} = "ModemManager.service"

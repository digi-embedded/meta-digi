DESCRIPTION = "A Client for Wi-Fi Protected Access (WPA)."
HOMEPAGE = "http://hostap.epitest.fi/wpa_supplicant/"
BUGTRACKER = "http://hostap.epitest.fi/bugz/"
SECTION = "network"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://../COPYING;md5=ab87f20cd7e8c0d0a6539b34d3791d0e \
                    file://../README;md5=5cb758942d25f6f61fd4ac388fd446fa \
                    file://wpa_supplicant.c;beginline=1;endline=12;md5=cba4fa09fa364da845ca546f21008909"

DEPENDS = "libnl openssl"

INC_PR = "r0"

SRC_URI = "http://hostap.epitest.fi/releases/wpa_supplicant-${PV}.tar.gz \
           file://defconfig \
           file://wpa_supplicant.conf \
           file://wpa-supplicant.sh \
           file://99_wpa_supplicant"

S = "${WORKDIR}/wpa_supplicant-${PV}/wpa_supplicant"

do_configure () {
	echo "CFLAGS +=\"-I${STAGING_INCDIR}/libnl3\"" >> ${WORKDIR}/defconfig
	install -m 0644 ${WORKDIR}/defconfig .config
}

export EXTRA_CFLAGS = "${CFLAGS}"
export BINDIR = "${sbindir}"

do_compile () {
	unset CFLAGS CPPFLAGS CXXFLAGS
	oe_runmake
}

do_install () {
	install -d ${D}${sbindir}
	install -m 0755 wpa_supplicant ${D}${sbindir}
	install -m 0755 wpa_cli ${D}${sbindir}

	install -d ${D}${bindir}
	install -m 0755 wpa_passphrase ${D}${bindir}

	install -d ${D}${sysconfdir}
	install -m 0600 ${WORKDIR}/wpa_supplicant.conf ${D}${sysconfdir}/wpa_supplicant.conf

	install -d ${D}${sysconfdir}/network/if-pre-up.d/
	install -d ${D}${sysconfdir}/network/if-post-down.d/
	install -d ${D}${sysconfdir}/network/if-down.d/
	install -m 0755 ${WORKDIR}/wpa-supplicant.sh ${D}${sysconfdir}/network/if-pre-up.d/wpa-supplicant
	cd ${D}${sysconfdir}/network/ && ln -sf ../if-pre-up.d/wpa-supplicant if-post-down.d/wpa-supplicant

	install -d ${D}/etc/default/volatiles
	install -m 0644 ${WORKDIR}/99_wpa_supplicant ${D}/etc/default/volatiles
}

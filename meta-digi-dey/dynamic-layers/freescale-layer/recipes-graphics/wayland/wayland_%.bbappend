# Copyright (C) 2020-2022, Digi International Inc.

FILES:${PN}:class-nativesdk = "${bindir}/* ${sbindir}/* ${libexecdir}/* ${libdir}/lib*${SOLIBS} \
            ${sysconfdir} ${sharedstatedir} ${localstatedir} \
            ${base_bindir}/* ${base_sbindir}/* \
            ${base_libdir}/*${SOLIBS} \
            ${base_prefix}/lib/udev ${prefix}/lib/udev \
            ${base_libdir}/udev ${libdir}/udev \
            ${datadir}/${BPN} ${libdir}/${BPN}/* \
            ${datadir}/pixmaps ${datadir}/applications \
            ${datadir}/idl ${datadir}/omf ${datadir}/sounds \
            ${libdir}/bonobo/servers"
FILES:${PN}-dev:remove:class-nativesdk = "${bindir} ${datadir}/wayland"

FILES_SOLIBSDEV = " \
    ${base_libdir}/lib*${SOLIBSDEV} \
    ${libdir}/libwayland-client.so \
    ${libdir}/libwayland-cursor.so \
    ${libdir}/libwayland-server.so \
"
FILES:${PN} += "${libdir}/libwayland-egl.so"

RPROVIDES:${PN} = "libwayland-egl.so()(64bit)"

INSANE_SKIP:${PN} += "dev-so"

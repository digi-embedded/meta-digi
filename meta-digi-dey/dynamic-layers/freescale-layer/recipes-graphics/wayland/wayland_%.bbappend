# Copyright (C) 2020-2023, Digi International Inc.

FILES_SOLIBSDEV = " \
    ${base_libdir}/lib*${SOLIBSDEV} \
    ${libdir}/libwayland-client.so \
    ${libdir}/libwayland-cursor.so \
    ${libdir}/libwayland-server.so \
"
FILES:${PN} += "${libdir}/libwayland-egl.so"

RPROVIDES:${PN} = "libwayland-egl.so()(64bit)"

INSANE_SKIP:${PN} += "dev-so"

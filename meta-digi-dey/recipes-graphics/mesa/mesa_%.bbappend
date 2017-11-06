# Copyright (C) 2017 Digi International

#
# Add runtime dependency so that GLES3 headers don't need to be added manually
#
RDEPENDS_libgles2-mesa-dev += "libgles3-mesa-dev"

#
# Add platform dependency to maintain compatibility
# with GPU driver previous to v6.
#
PROVIDES_remove = "gbm"
PACKAGECONFIG_remove = "gbm"

BACKEND = \
    "${@bb.utils.contains('DISTRO_FEATURES', 'wayland', 'wayland', \
        bb.utils.contains('DISTRO_FEATURES',     'x11',     'x11', \
                                                             'fb', d), d)}"

do_install_append () {
    rm -f ${D}${includedir}/GL/glx.h \
          ${D}${includedir}/GL/glxext.h
    if [ "${BACKEND}" = "x11" ]; then
        rm -f ${D}${libdir}/pkgconfig/gl.pc
    fi
}

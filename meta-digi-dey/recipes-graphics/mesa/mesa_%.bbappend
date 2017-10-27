# Copyright (C) 2017 Digi International

#
# Add runtime dependency so that GLES3 headers don't need to be added manually
#
RDEPENDS_libgles2-mesa-dev += "libgles3-mesa-dev"

#
# Add platform dependency to maintain compatibility
# with GPU driver previous to v6.
#
PROVIDES_remove_ccimx6qpsbc = "gbm"
PACKAGECONFIG_remove_ccimx6qpsbc = "gbm"

BACKEND = \
    "${@bb.utils.contains('DISTRO_FEATURES', 'wayland', 'wayland', \
        bb.utils.contains('DISTRO_FEATURES',     'x11',     'x11', \
                                                             'fb', d), d)}"

do_install_append_ccimx6qpsbc () {
    rm -f ${D}${includedir}/GL/glx.h \
          ${D}${includedir}/GL/glxext.h
    if [ "${BACKEND}" = "x11" ]; then
        rm -f ${D}${libdir}/pkgconfig/gl.pc
    fi
}

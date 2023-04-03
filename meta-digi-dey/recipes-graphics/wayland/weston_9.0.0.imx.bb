require recipes-graphics/wayland/weston_9.0.0.bb

SRC_URI_remove = "https://wayland.freedesktop.org/releases/${BPN}-${PV}.tar.xz \
                  file://0001-tests-include-fcntl.h-for-open-O_RDWR-O_CLOEXEC-and-.patch"
WESTON_SRC ?= "git://github.com/nxp-imx/weston-imx.git;protocol=https"
SRC_URI_prepend = "${WESTON_SRC};branch=weston-imx-9.0 "
SRCREV = "230e9bc3d647e511e0601e3d45034f22495ed3c7"
S = "${WORKDIR}/git"

# Disable OpenGL for parts with GPU support for 2D but not 3D
REQUIRED_DISTRO_FEATURES_imxgpu2d = ""
REQUIRED_DISTRO_FEATURES_imxgpu3d = "opengl"
PACKAGECONFIG_OPENGL              = "opengl"
PACKAGECONFIG_OPENGL_imxgpu2d     = ""
PACKAGECONFIG_OPENGL_imxgpu3d     = "opengl"

PACKAGECONFIG_remove = "wayland x11"
PACKAGECONFIG_append = " ${@bb.utils.filter('DISTRO_FEATURES', '${PACKAGECONFIG_OPENGL}', d)}"
PACKAGECONFIG_remove_imxfbdev = "kms"
PACKAGECONFIG_append_imxgpu   = " imxgpu"
PACKAGECONFIG_append_imxgpu2d = " g2d"
#PACKAGECONFIG_append_imxgpu3d = " cairo-glesv2"

# Weston with Xwayland support (requires X11 and Wayland)
PACKAGECONFIG[xwayland] = "-Dxwayland=true,-Dxwayland=false,libxcursor"
# Clients support
SIMPLE_CLIENTS = "all"
SIMPLE_CLIENTS_imxfbdev = "damage,im,egl,shm,touch,dmabuf-v4l"
PACKAGECONFIG[clients] = "-Dsimple-clients=${SIMPLE_CLIENTS} -Ddemo-clients=true,-Dsimple-clients= -Ddemo-clients=false"
# Weston with cairo glesv2 support
#PACKAGECONFIG[cairo-glesv2] = "-Dcairo-glesv2=true,-Dcairo=image"
# Weston with i.MX GPU support
PACKAGECONFIG[imxgpu] = "-Dimxgpu=true,-Dimxgpu=false"
# Weston with i.MX G2D renderer
PACKAGECONFIG[g2d] = "-Drenderer-g2d=true,-Drenderer-g2d=false,virtual/libg2d"
# Weston with OpenGL support
PACKAGECONFIG[opengl] = "-Dopengl=true,-Dopengl=false"

FILES_${PN} += "${libdir}/${BPN}/lib*${SOLIBS}"
FILES_${PN}-dbg += "${libdir}/${BPN}/libexec_weston${SOLIBSDEV}"

PACKAGE_ARCH = "${MACHINE_SOCARCH}"
COMPATIBLE_MACHINE = "(imxfbdev|imxgpu)"

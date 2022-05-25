# Undo customization in meta-freescale that doesn't apply to 8DXL
PACKAGECONFIG:remove:mx8dxl = "osmesa"
DRIDRIVERS:remove:mx8dxl = "swrast"
PACKAGECONFIG:remove:mx8phantomdxl = "osmesa"
DRIDRIVERS:remove:mx8phantomdxl = "swrast"
PACKAGECONFIG:remove:mx8mnlite = "osmesa"
DRIDRIVERS:remove:mx8mnlite = "swrast"

do_install:append:imxgpu3d () {
    rm -f ${D}${includedir}/GL/glcorearb.h
}

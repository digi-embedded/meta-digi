# Digi: NXP enables the openxr config here to mirror the source default, only
# to disable it in a separate file (fsl-imx-base.inc) in their meta-imx-sdk
# layer. Since we don't include this layer, simply disable the config here by
# not adding it to PACKAGECONFIG.
#PACKAGECONFIG:class-target:append = " openxr"

PACKAGECONFIG[openxr] = "-DFEATURE_quick3dxr_openxr=ON,-DFEATURE_quick3dxr_openxr=OFF"

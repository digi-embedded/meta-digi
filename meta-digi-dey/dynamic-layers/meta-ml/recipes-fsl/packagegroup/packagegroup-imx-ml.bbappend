# Copyright 2023 Digi International Inc.

ML_NNSTREAMER_PKGS_LIST:remove = "nnstreamer-deepview-rt"

ML_PKGS:mx9-nxp-bsp:remove = "deepview-rt-examples"
ML_PKGS:mx9-nxp-bsp:append = " modelrunner"

# ARM ethos-u package
ETHOS_U_PKGS:append:mx93-nxp-bsp = " \
    eiq-examples \
    tensorflow-lite-ethosu-delegate \
"

# Copyright 2023,2024 Digi International Inc.

ML_NNSTREAMER_PKGS_LIST:remove = "nnstreamer-deepview-rt"

ML_PKGS:mx9-nxp-bsp:remove = "deepview-rt-examples"

# ARM ethos-u package
ETHOS_U_PKGS:append:mx93-nxp-bsp = " \
    eiq-examples \
    tensorflow-lite-ethosu-delegate \
"

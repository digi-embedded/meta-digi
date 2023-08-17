# Copyright (C) 2023 Digi International

SRCBRANCH_runtime:ccimx93 = "lf-6.1.1_1.0.0"
SRCREV_runtime:ccimx93 = "66e3e9a93840ed1e55dc2d7e894c0ae26fb0e51e"

# Updated flatbuffers recipe for ccimx93, renamed the runtime python package
PYTHON_RDEPENDS:remove:ccimx93 = "flatbuffers-${PYTHON_PN}"
PYTHON_RDEPENDS:append:ccimx93 = " ${PYTHON_PN}-flatbuffers"

# Copyright (C) 2019 Digi International

require trustfence-cst.inc

CST_EXTENSION = "tgz"

S = "${WORKDIR}/release"

INSANE_SKIP_${PN} += "already-stripped"

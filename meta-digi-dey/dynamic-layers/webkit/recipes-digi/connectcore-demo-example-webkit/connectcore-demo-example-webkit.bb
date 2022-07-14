# Copyright (C) 2022 Digi International.

require recipes-digi/dey-examples/connectcore-demo-example.inc

RDEPENDS:${PN} += " \
    cog \
    video-examples \
    webglsamples \
"

RREPLACES:${PN} = "connectcore-demo-example"
RCONFLICTS:${PN} = "connectcore-demo-example"

# Copyright (C) 2013 Digi International.

PRINC := "${@int(PRINC) + 1}"
PR_append = "+${DISTRO}"

EXTRA_OECONF += " \
    --disable-dvdlpcmdec \
    --disable-dvdsub \
    --disable-iec958 \
    --disable-rpath \
"

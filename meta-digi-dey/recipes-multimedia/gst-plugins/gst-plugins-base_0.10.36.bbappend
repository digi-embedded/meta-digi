# Copyright (C) 2013 Digi International.

PRINC := "${@int(PRINC) + 1}"
PR_append = "+${DISTRO}"

PACKAGECONFIG ??= "${@base_contains('DISTRO_FEATURES', 'x11', 'x11', '', d)}"
PACKAGECONFIG[x11] = ",--disable-x --disable-xshm --disable-xvideo --disable-libvisual,"

EXTRA_OECONF += " \
    --disable-cdparanoia \
    --disable-examples \
    --disable-gtk-doc \
    --disable-ivorbis \
    --disable-rpath \
"

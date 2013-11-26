# Copyright (C) 2013 Digi International.

PRINC := "${@int(PRINC) + 1}"
PR_append = "+${DISTRO}"

# Disable integer vorbis plugin as it conflicts with other vorbis plugin with
# error: GLib-GObject-WARNING **: cannot register existing type `GstVorbisDec'
EXTRA_OECONF += "--disable-ivorbis"

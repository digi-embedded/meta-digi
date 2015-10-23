# Copyright (C) 2013 Digi International.

# Disable integer vorbis plugin as it conflicts with other vorbis plugin with
# error: GLib-GObject-WARNING **: cannot register existing type `GstVorbisDec'
EXTRA_OECONF += "--disable-ivorbis"

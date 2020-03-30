# Copyright (C) 2020 Digi International Inc.

PV = "1.16.0"

SRC_URI[md5sum] = "adc4460239ec2eccf58ad9752ce53bfd"
SRC_URI[sha256sum] = "198e9eec1a3e32dc810d3fbf3a714850a22c6288d4a5c8e802c5ff984af03f19"

# Disable introspection to fix [GstPlayer-1.0.gir] Error
EXTRA_OECONF += " \
    --disable-introspection \
"


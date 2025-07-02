# Copyright (C) 2014-2025, Digi International Inc.

#
# DEY only uses x11-sato based images on the CC6UL, so tweak the
# recipe to fit in the small CC6UL rootfs.
#

# Disable GTK-based gstreamer examples and network manager
GSTEXAMPLES:dey = ""
NETWORK_MANAGER:dey = ""

# Prevent installing ICU package
RDEPENDS:${PN}-apps:remove:dey = "matchbox-terminal"

# Prevent launching a second instance of pulseaudio as
# DEY uses system-wide pulseaudio
RDEPENDS:${PN}-base:remove:dey = "pulseaudio-client-conf-sato"

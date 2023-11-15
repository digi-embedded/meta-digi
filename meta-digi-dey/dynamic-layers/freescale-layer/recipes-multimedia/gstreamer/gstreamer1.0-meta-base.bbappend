# Copyright 2023 Digi International Inc.

RDEPENDS:gstreamer1.0-meta-base:remove:ccimx93 = " \
    gstreamer1.0-plugins-base-videoscale \
    gstreamer1.0-plugins-base-videoconvert \
"
RDEPENDS:gstreamer1.0-meta-base:append:ccimx93 = " \
    gstreamer1.0-plugins-base-videoconvertscale \
"

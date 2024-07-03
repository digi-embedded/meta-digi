# Copyright (C) 2023,2024, Digi International Inc.

RDEPENDS:gstreamer1.0-meta-base:remove:ccimx9 = " \
    gstreamer1.0-plugins-base-videoscale \
    gstreamer1.0-plugins-base-videoconvert \
"
RDEPENDS:gstreamer1.0-meta-base:append:ccimx9 = " \
    gstreamer1.0-plugins-base-videoconvertscale \
"

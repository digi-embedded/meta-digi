# Copyright (C) 2020-2025, Digi International Inc.

# We can't build the WebKit with fb images, so force wayland as a required
# distro feature.
inherit features_check

REQUIRED_DISTRO_FEATURES = "wayland"

# Limit number of parallel threads make can run to avoid a ninja build issue
PARALLEL_MAKE = "-j ${@oe.utils.cpu_count(at_most=8)}"

# Remove PACKAGECONFIGs that either no longer work or pull in unwanted
# dependencies
PACKAGECONFIG:remove = " \
    accessibility \
    lbse \
    openjpeg \
    service-worker \
    speech-synthesis \
"

# gbm PACKAGECONFIG pulls in libgbm dependency, which isn't available
# on the i.MX6 and ccmp1
PACKAGECONFIG:remove:ccimx6 = "gbm"
PACKAGECONFIG:remove:ccmp1 = "gbm"

# If BBLAYERS contains meta-qt6, the wpewebkit recipe inherits the qt6-cmake
# bbclass, which is necessary if the qtwpe PACKAGECONFIG is enabled. However,
# even if this config is disabled, the bbclass automatically adds a dependency
# with qtbase-native. Only keep this dependency if we enable qtwpe, and remove
# it otherwise.
DEPENDS:remove = "${@bb.utils.contains('PACKAGECONFIG', 'qtwpe', '', 'qtbase-native', d)}"

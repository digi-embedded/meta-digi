# Copyright (C) 2020-2024, Digi International Inc.

EXTRA_OECMAKE += "-DCOG_HOME_URI=http://127.0.0.1/"

# drm PACKAGECONFIG pulls in libgbm dependency, which isn't available
# on the i.MX6 and ccmp1
PACKAGECONFIG:remove:ccimx6 = "drm"
PACKAGECONFIG:remove:ccmp1 = "drm"

# Copyright (C) 2022, Digi International Inc.

# In sysvinit builds, cups pulls in procps as a dependency, which causes
# conflicts when building the SDK, so remove it
PACKAGECONFIG:remove = "${@bb.utils.contains('DISTRO_FEATURES', 'sysvinit', 'cups', '', d)}"

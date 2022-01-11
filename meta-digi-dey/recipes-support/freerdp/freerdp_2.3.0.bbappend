# Copyright (C) 2022 Digi International.

# In sysvinit builds, cups pulls in procps as a dependency, which causes
# conflicts when building the SDK, so remove it
PACKAGECONFIG_remove = "${@bb.utils.contains('DISTRO_FEATURES', 'sysvinit', 'cups', '', d)}"

# cups pulls in libusb1 as a dependency, but libusb1 is also needed implicitly
# by a different freerdp component. Removing cups from PACKAGECONFIG will
# remove this dependency and cause the build to fail, so we need make the
# dependency explicit in this scenario.
DEPENDS += " ${@bb.utils.contains('DISTRO_FEATURES', 'sysvinit', 'libusb1', '', d)}"

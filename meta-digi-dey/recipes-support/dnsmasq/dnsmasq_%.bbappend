# Copyright (C) 2017, Digi International Inc.

# Enable DBUS support so it can be used from NetworkManager
PACKAGECONFIG:append = " dbus"

# NetworkManager will launch 'dnsmasq' using DBUS, so disable the creation
# of runlevel's symlinks and disable its systemd service.
INHIBIT_UPDATERCD_BBCLASS = "1"
SYSTEMD_AUTO_ENABLE = "disable"

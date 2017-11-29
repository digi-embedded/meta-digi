# Copyright (C) 2017, Digi International Inc.

# Enable DBUS support so it can be used from NetworkManager
PACKAGECONFIG_append = " dbus"

# NetworkManager will launch 'dnsmasq' using DBUS, so disable the creation
# of runlevel's symlinks.
INHIBIT_UPDATERCD_BBCLASS = "1"

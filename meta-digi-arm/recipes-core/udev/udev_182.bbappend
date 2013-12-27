# Copyright (C) 2013 Digi International.

do_install_append () {
    # On the fly fix udev init script and config file to avoid udev failing
    # on boot a read-only NFSROOT:
    #   bind failed: No such file or directory
    #   bind failed: Address already in use
    #   error binding udev control socket
    sed -i -e '/mkdir.*pts/a\    [ -e /dev/run ] || mkdir -m 0755 /dev/run' ${D}${sysconfdir}/init.d/udev
    sed -i -e '/^udev_run=/c\udev_run="/dev/run/udev"' ${D}${sysconfdir}/udev/udev.conf
}

# Add 'udev-extraconf' recommendation as most of the imx/mxs settings are in
# that package.
RRECOMMENDS_${PN} += "udev-extraconf"

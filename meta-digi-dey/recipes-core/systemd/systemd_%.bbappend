FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI += " \
    file://0001-sysv-generator-reduce-message-level-for-packages-tha.patch \
    file://0002-sd-resolve-forcefully-cancel-worker-threads-during-r.patch \
"

# Remove systemd-networkd from our images, since we already use NetworkManager
PACKAGECONFIG:remove:dey = "networkd"

do_install:append () {
    # Disable the assignment of the fixed network interface name
    install -d ${D}${sysconfdir}/systemd/network
    ln -s /dev/null ${D}${sysconfdir}/systemd/network/99-default.link

    # Add special touchscreen rules
    if [ -e  ${D}${sysconfdir}/udev/rules.d/touchscreen.rules ]; then
        cat <<EOF >>${D}${sysconfdir}/udev/rules.d/touchscreen.rules
# i.MX specific touchscreen rules
SUBSYSTEM=="input", KERNEL=="event[0-9]*", ENV{ID_INPUT_TOUCHSCREEN}=="1", SYMLINK+="input/touchscreen0"
EOF
    fi

    # Disable virtual terminals
    if [ "${USE_VT}" = "0" ]; then
        sed -i -e "/getty@.service/s,enable,disable,g" ${D}${systemd_unitdir}/system-preset/90-systemd.preset
    fi
}

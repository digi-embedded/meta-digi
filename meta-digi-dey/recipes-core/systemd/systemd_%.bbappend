FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI += " \
    file://0001-udev-use-the-usual-set-of-load-paths-for-udev-rules.patch \
    file://0002-sd-resolve-forcefully-cancel-worker-threads-during-r.patch \
"

#FIX-it: Workaround as missing ending slash in FIRMWARE_PATH [YOCIMX-2831]
EXTRA_OEMESON_remove = "-Dfirmware-path=${nonarch_base_libdir}/firmware "
EXTRA_OEMESON   += "-Dfirmware-path=${nonarch_base_libdir}/firmware/ "

do_install_append () {
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
}

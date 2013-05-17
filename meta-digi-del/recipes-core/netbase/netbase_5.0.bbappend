# Copyright (C) 2013 Digi International.

PRINC := "${@int(PRINC) + 1}"
PR_append = "+${DISTRO}"

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}-${PV}:"

DEPENDS="virtual/kernel"

SRC_URI += "file://ifup"

WPA_DRIVER ?= "wext"

do_install_append(){
	install -m 0755 ${WORKDIR}/ifup ${D}${sysconfdir}/network/if-up.d
}

pkg_postinst_${PN} () {
#!/bin/sh
INTERFACES_PATH=$D/etc/network/interfaces
if test "x$D" != "x"; then
	KERNEL_CONFIG_PATH=${STAGING_KERNEL_DIR}/.config
else
	KERNEL_CONFIG_PATH=/boot/config*
fi

> ${INTERFACES_PATH}

/bin/grep -q "CONFIG_BLK_DEV_LOOP=" ${KERNEL_CONFIG_PATH}
if [ $? -eq 0 ]; then
cat << EOF >> ${INTERFACES_PATH}
# The loopback interface
auto lo
iface lo inet loopback
EOF
fi

/bin/grep -q  "CONFIG_FEC=" ${KERNEL_CONFIG_PATH}
if [ $? -eq 0 ]; then
# Primary wired interface
cat << EOF >> ${INTERFACES_PATH}
auto eth0
# Use for dhcp
# iface eth0 inet dhcp
iface eth0 inet static
    address 192.168.42.30
    netmask 255.255.255.0
    network 192.168.42.0
    gateway 192.168.42.1

EOF
fi

# Secondary wired interface on MXC platforms
/bin/grep -q "CONFIG_SMSC911X=" ${KERNEL_CONFIG_PATH}
if [ $? -eq 0 ]; then
cat << EOF >> ${INTERFACES_PATH}
auto eth1
# Use for dhcp
# iface eth1 inet dhcp
iface eth1 inet static
    address 192.168.44.30
    netmask 255.255.255.0
    network 192.168.44.0
    gateway 192.168.44.1

EOF
fi

# Secondary wired interface on MXS platforms
/bin/grep -q  "CONFIG_CCARDIMX28_ENET1=" ${KERNEL_CONFIG_PATH}
if [ $? -eq 0 ]; then
cat << EOF >> ${INTERFACES_PATH}
auto eth1
# Use for dhcp
# iface eth1 inet dhcp
iface eth1 inet static
    address 192.168.44.30
    netmask 255.255.255.0
    network 192.168.44.0
    gateway 192.168.44.1

EOF
fi

# Wireless interface
/bin/grep -q  "CONFIG_WIRELESS=" ${KERNEL_CONFIG_PATH}
if [ $? -eq 0 ]; then
cat << EOF >> ${INTERFACES_PATH}
auto wlan0
# Use for dhcp
# iface wlan0 inet dhcp
iface wlan0 inet static
    address 192.168.43.30
    netmask 255.255.255.0
    network 192.168.43.0
    wireless_mode managed
    wireless_essid any
    wpa-driver ${WPA_DRIVER}
    wpa-conf /etc/wpa_supplicant.conf

EOF
fi
}

CONFFILES_${PN} = "${sysconfdir}/hosts"

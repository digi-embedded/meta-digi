FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}-${PV}:"

PR_append = "+${DISTRO}.r0"

DEPENDS="virtual/kernel"

SRC_URI += "file://ifup"

WPA_DRIVER ?= "wext"

do_install_append(){
	install -m 0755 ${WORKDIR}/ifup ${D}${sysconfdir}/network/if-up.d
}

pkg_postinst_${PN} () {
#!/bin/sh
cat << EOF > $D${sysconfdir}/network/interfaces
EOF

cat $D/boot/config* | /bin/grep CONFIG_BLK_DEV_LOOP=y
if [ $? -eq 0 ]; then
cat << EOF >> $D${sysconfdir}/network/interfaces
# The loopback interface
auto lo
iface lo inet loopback
EOF
fi

cat $D/boot/config* | /bin/grep CONFIG_FEC=y
if [ $? -eq 0 ]; then
# Primary wired interface
cat << EOF >> $D${sysconfdir}/network/interfaces
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

# Secondary wired interface
cat $D/boot/config* | /bin/grep CONFIG_SMSC911X=y
if [ $? -eq 0 ]; then
cat << EOF >> $D${sysconfdir}/network/interfaces
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
cat $D/boot/config* | /bin/grep CONFIG_WIRELESS=y
if [ $? -eq 0 ]; then
cat << EOF >> $D${sysconfdir}/network/interfaces
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

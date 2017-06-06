# Copyright (C) 2017, Digi International Inc.

SUMMARY = "AWS IoT Greengrass core"
HOMEPAGE = "https://aws.amazon.com/greengrass/"
#
# The package includes different licenses:
#
# [Apache-2.0]
#   LICENSE/github_aws_aws_sdk_go_License.txt
#   LICENSE/github_coreos_go_systemd_License.txt
#   LICENSE/github_docker_docker_License.txt
#   LICENSE/github_docker_go_units_License.txt
#   LICENSE/github_go_ini_ini_License.txt
#   LICENSE/github_jmespath_go_jmespath_License.txt
#   LICENSE/github_opencontainers_runc_License.txt
#   LICENSE/github_opencontainers_runtime_spec_License.txt
#   LICENSE/github_pquerna_ffjson_License.txt
#   LICENSE/github_vishvananda_netlink_License.txt
# [BSD-2-Clause]
#   LICENSE/github_godbus_dbus_License.txt
#   LICENSE/github_huin_gobinarytest_License.txt
#   LICENSE/github_seccomp_libseccomp_golang_License.txt
#   LICENSE/github_syndtr_gocapability_License.txt
# [BSD-3-Clause]
#   LICENSE/github_golang_protobuf_License.txt
#   LICENSE/github_jeffallen_mqtt_License.txt
#   LICENSE/Golang_License.txt
# [MIT]
#   LICENSE/github_huin_mqtt_License.txt
#   LICENSE/github_mattn_go_sqlite3_License.txt
#   LICENSE/github_nu7hatch_gouuid_License.txt
#   LICENSE/github_Sirupsen_logrus_License.txt
#   LICENSE/github_urfave_cli_License.txt
#   LICENSE/github_yosssi_gmq_License.txt
# [Proprietary]
#   LICENSE/GG-BetaMaterials-License.txt
#
LICENSE = "Apache-2.0 | BSD-2-Clause | BSD-3-Clause | MIT | Proprietary"
LIC_FILES_CHKSUM = " \
    file://LICENSE/GG-BetaMaterials-License.txt;md5=a33101714f905fe97db6fe085c1271ef \
    file://LICENSE/github_aws_aws_sdk_go_License.txt;md5=d273d63619c9aeaf15cdaf76422c4f87 \
    file://LICENSE/github_coreos_go_systemd_License.txt;md5=715f3348ed8b9bf4fac3b08133384a4d \
    file://LICENSE/github_docker_docker_License.txt;md5=bba4ee48af378e39b452d742d29c710b \
    file://LICENSE/github_docker_go_units_License.txt;md5=bb99db20f1c48c2c4952c27c72855e36 \
    file://LICENSE/github_godbus_dbus_License.txt;md5=b03a62440372a9acf9692ad365932c87 \
    file://LICENSE/github_go_ini_ini_License.txt;md5=715f3348ed8b9bf4fac3b08133384a4d \
    file://LICENSE/github_golang_protobuf_License.txt;md5=16fe162f7848190010b6ec7bfaac030a \
    file://LICENSE/github_huin_gobinarytest_License.txt;md5=f2b3138d9d314bccf5297dea7e3e6d14 \
    file://LICENSE/github_huin_mqtt_License.txt;md5=12fd125064676697934b7d8c09bed0e8 \
    file://LICENSE/github_jeffallen_mqtt_License.txt;md5=b7269d52765d477e10f319c19d8a9d33 \
    file://LICENSE/github_jmespath_go_jmespath_License.txt;md5=640d33f0070c9dc3a194d2ed7db02974 \
    file://LICENSE/github_mattn_go_sqlite3_License.txt;md5=948f36a2300ac729e60416063190f664 \
    file://LICENSE/github_nu7hatch_gouuid_License.txt;md5=6b18748dcc29fda05fa5aaef44d517fd \
    file://LICENSE/github_opencontainers_runc_License.txt;md5=587c01b2dcc5dc3b4bed51b918c64731 \
    file://LICENSE/github_opencontainers_runtime_spec_License.txt;md5=ef95ed297310c3d09ba16c06d5e161a5 \
    file://LICENSE/github_pquerna_ffjson_License.txt;md5=d273d63619c9aeaf15cdaf76422c4f87 \
    file://LICENSE/github_seccomp_libseccomp_golang_License.txt;md5=9205c4c469bfb9d3a63f346539ee445b \
    file://LICENSE/github_Sirupsen_logrus_License.txt;md5=29baae91637760ae68feb57ca93e5a0a \
    file://LICENSE/github_syndtr_gocapability_License.txt;md5=321f58fa53a0b1bb9a887f14660d436b \
    file://LICENSE/github_urfave_cli_License.txt;md5=f1f14a2449300559aed90bedc36a71ed \
    file://LICENSE/github_vishvananda_netlink_License.txt;md5=c95fd0efd62139c155e956a448df8fd6 \
    file://LICENSE/github_yosssi_gmq_License.txt;md5=2509f45544da1ecce869ce2de1aa44dd \
    file://LICENSE/Golang_License.txt;md5=3d7ed06383c65a3161b36c6a0b0b98f5 \
"

SRC_URI = " \
    http:///not/exist/greengrass-linux-armv6l-${PV}-release.tar.gz \
    file://0001-greengrassd-remove-bashisms-in-launcher-shell-script.patch \
"
SRC_URI[md5sum] = "eb7e6dbdfe00e51db8b7ffbd2284ae59"
SRC_URI[sha256sum] = "24da4016345eeeb6a86067619d385015139437fbd16dba9a91461758692f933f"

GG_TARBALL_LOCAL_PATH ?= ""

# The tarball is only available for downloading after registration, so provide
# a PREMIRROR to a local directory that can be configured in the project's
# local.conf file using GG_TARBALL_LOCAL_PATH variable.
python() {
    gg_tarball_local_path = d.getVar('GG_TARBALL_LOCAL_PATH', True)
    if gg_tarball_local_path:
        premirrors = d.getVar('PREMIRRORS', True)
        d.setVar('PREMIRRORS', "http:///not/exist/greengrass.* file://%s \\n %s" % (gg_tarball_local_path, premirrors))
}

S = "${WORKDIR}/${BPN}"

inherit useradd

GG_USESYSTEMD = "${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'yes', 'no', d)}"

# Disable tasks not needed for the binary package
do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install() {
	install -d ${D}/${BPN}
	tar --no-same-owner --exclude='./patches' --exclude='./.pc' -cpf - -C ${S} . \
		| tar --no-same-owner -xpf - -C ${D}/${BPN}

	# Configure whether to use systemd or not
	sed -i -e "/useSystemd/{s,\[yes|no],${GG_USESYSTEMD},g}" ${D}/${BPN}/configuration/config.json
}

pkg_postinst_${PN}() {
	# Enable protection for hardlinks and symlinks
	if ! grep -qs 'protected_.*links' $D${sysconfdir}/sysctl.conf; then
		cat >> $D${sysconfdir}/sysctl.conf <<-_EOF_
			# Greengrass: protect hardlinks/symlinks
			fs.protected_hardlinks = 1
			fs.protected_symlinks = 1
		_EOF_
	fi

	# Customize '/etc/fstab'
	if [ -f "$D${sysconfdir}/fstab" ]; then
		# Disable TMPFS /var/volatile
		sed -i -e '\#^tmpfs[[:blank:]]\+/var/volatile#s,^,#,g' $D${sysconfdir}/fstab

		# Mount a cgroup hierarchy with all available subsystems
		if ! grep -qs '^cgroup' $D${sysconfdir}/fstab; then
			cat >> $D${sysconfdir}/fstab <<-_EOF_
				# Greengrass: mount cgroups
				cgroup    /sys/fs/cgroup    cgroup    defaults    0  0
			_EOF_
		fi
	fi

	# Disable '/etc/resolv.conf' symlink
	if [ -f "$D${sysconfdir}/default/volatiles/00_core" ]; then
		sed -i -e '/resolv.conf/d' $D${sysconfdir}/default/volatiles/00_core
		cat >> $D${sysconfdir}/default/volatiles/00_core <<-_EOF_
			# Greengrass: create a real (no symlink) resolv.conf
			f root root 0644 /etc/resolv.conf none
		_EOF_
	fi
}

FILES_${PN} = "/${BPN}"

USERADD_PACKAGES = "${PN}"
GROUPADD_PARAM_${PN} = "-r ggc_group"
USERADD_PARAM_${PN} = "-r -M -N -g ggc_group -s /bin/false ggc_user"

#
# Disable failing QA checks:
#
#   Binary was already stripped
#   No GNU_HASH in the elf binary
#
INSANE_SKIP_${PN} += "already-stripped ldflags"

RDEPENDS_${PN} += "ca-certificates python-argparse python-json python-numbers sqlite3"

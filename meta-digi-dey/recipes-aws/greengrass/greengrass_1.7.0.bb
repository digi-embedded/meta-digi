# Copyright (C) 2018, Digi International Inc.

SUMMARY = "AWS IoT Greengrass core"
HOMEPAGE = "https://aws.amazon.com/greengrass/"
#
# The Amazon Greengrass Core Product includes the following third-party software/licensing:
# github.com/aws/aws-sdk-go/; version 1.15.65 -- https://github.com/aws/aws-sdk-go/
# github.com/coreos/go-systemd/; version 10 -- https://github.com/coreos/go-systemd/
# github.com/docker/docker; version 1.12.0-rc4 -- https://github.com/docker/docker
# github.com/docker/go-units; version 0.3.1 -- https://github.com/docker/go-units
# github.com/go-ini/ini; version 1.32.0 -- https://github.com/go-ini/ini
# github.com/jmespath/go-jmespath; version 0.2.2 -- https://github.com/jmespath/go-jmespath
# github.com/mwitkow/go-http-dialer; version 0.1 -- https://github.com/mwitkow/go-http-dialer
# github.com/opencontainers/runc; version 1.0.0-rc3 -- https://github.com/opencontainers/runc
# github.com/opencontainers/runtime-spec; version 1.0.0-rc5 -- https://github.com/opencontainers/runtime-spec
# github.com/pquerna/ffjson; version 1.0 -- https://github.com/pquerna/ffjson
# github.com/vishvananda/netlink; version 0.1 -- https://github.com/vishvananda/netlink
#
# And the following Licenses:
LICENSE = "Apache-2.0 | BSD-2-Clause | BSD-3-Clause | MIT | PD | Proprietary"
LIC_FILES_CHKSUM = " \
    file://ggc/core/THIRD-PARTY-LICENSES;md5=28584ceb716d242782f9a7a7593c9ff2 \
"
SRC_URI_arm = " \
    http:///not/exist/greengrass-linux-armv7l-${PV}.tar.gz;name=arm \
    file://greengrass-init \
"

SRC_URI[arm.md5sum] = "a7f3667ac9f24e434e7a85908d1db256"
SRC_URI[arm.sha256sum] ="339656dca947f1cff29635fbe7570b5ea04ca7256fd2177cf396711a60a8f26a"

SRC_URI_aarch64 = " \
    http:///not/exist/greengrass-linux-aarch64-${PV}.tar.gz;name=aarch64 \
    file://greengrass-init \
"

# For ARCH64 we use another tarball.
SRC_URI[aarch64.md5sum] = "abfabf1464b7a1da0322dfd780415e48"
SRC_URI[aarch64.sha256sum] ="411956c8a41857c95dea5af6a41c7c0ab09310d621e054693d9e8ee57b23ed35"

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

inherit aws-iot update-rc.d useradd

GG_USESYSTEMD = "${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'yes', 'no', d)}"

# Disable tasks not needed for the binary package
do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install() {
	install -d ${D}/${BPN}
	tar --no-same-owner --exclude='./patches' --exclude='./.pc' -cpf - -C ${S} . \
		| tar --no-same-owner -xpf - -C ${D}/${BPN}

	# Install wrapper bootscript to launch Greengrass core on boot
	install -d ${D}${sysconfdir}/init.d
	install -m 0755 ${WORKDIR}/greengrass-init ${D}${sysconfdir}/init.d/greengrass
	sed -i -e "s,##GG_INSTALL_DIR##,/${BPN},g" ${D}${sysconfdir}/init.d/greengrass

	# If certificates do exist, install them and update the config file
	if [ -f "${AWS_IOT_CERTS_DIR}/${AWS_GGCORE_ROOT_CA}" ] && \
	   [ -f "${AWS_IOT_CERTS_DIR}/${AWS_GGCORE_CERTIFICATE}" ] && \
	   [ -f "${AWS_IOT_CERTS_DIR}/${AWS_GGCORE_PRIVATE_KEY}" ]; then
		install -m 0644 "${AWS_IOT_CERTS_DIR}/${AWS_GGCORE_ROOT_CA}" \
			"${AWS_IOT_CERTS_DIR}/${AWS_GGCORE_CERTIFICATE}" \
			"${AWS_IOT_CERTS_DIR}/${AWS_GGCORE_PRIVATE_KEY}" \
			${D}/${BPN}/certs/
		sed -i  -e "s,\[ROOT_CA_PEM_HERE],${AWS_GGCORE_ROOT_CA},g" \
			-e "s,\[CLOUD_PEM_CRT_HERE],${AWS_GGCORE_CERTIFICATE},g" \
			-e "s,\[CLOUD_PEM_KEY_HERE],${AWS_GGCORE_PRIVATE_KEY},g" \
			${D}/${BPN}/config/config.json
	fi

	# Configure the rest of GG Core parameters
	[ -n "${AWS_GGCORE_THING_ARN}" ] && sed -i -e "s,\[THING_ARN_HERE],${AWS_GGCORE_THING_ARN},g" ${D}/${BPN}/config/config.json
	if [ -n "${AWS_GGCORE_IOT_HOST}" ]; then
		AWS_GGCORE_HOST_PREFIX="$(echo ${AWS_GGCORE_IOT_HOST} | sed -e 's,\([^.]\+\)\.iot.*,\1,g')"
		AWS_GGCORE_REGION="$(echo ${AWS_GGCORE_IOT_HOST} | sed -e 's,.*.iot\.\([^.]\+\)\..*,\1,g')"
		[ -n "${AWS_GGCORE_HOST_PREFIX}" ] && sed -i -e "s,\[HOST_PREFIX_HERE],${AWS_GGCORE_HOST_PREFIX},g" ${D}/${BPN}/config/config.json
		[ -n "${AWS_GGCORE_REGION}" ] && sed -i -e "s,\[AWS_REGION_HERE],${AWS_GGCORE_REGION},g" ${D}/${BPN}/config/config.json
	fi

	# Configure whether to use systemd or not
	sed -i -e "/useSystemd/{s,\[yes|no],${GG_USESYSTEMD},g}" ${D}/${BPN}/config/config.json
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

FILES_${PN} = "/${BPN} ${sysconfdir}"

CONFFILES_${PN} += "/${BPN}/config/config.json"

INITSCRIPT_NAME = "greengrass"
INITSCRIPT_PARAMS = "defaults 80 20"

USERADD_PACKAGES = "${PN}"
GROUPADD_PARAM_${PN} = "-r ggc_group"
USERADD_PARAM_${PN} = "-r -M -N -g ggc_group -s /bin/false ggc_user"

#
# Disable failing QA checks:
#
#   Binary was already stripped
#   No GNU_HASH in the elf binary
#
INSANE_SKIP_${PN} += "already-stripped ldflags file-rdeps"

RDEPENDS_${PN} += "ca-certificates python-argparse python-json python-numbers sqlite3"

# Copyright (C) 2018, 2019, Digi International Inc.

SUMMARY = "AWS IoT Greengrass core"
HOMEPAGE = "https://aws.amazon.com/greengrass/"

CONFIGURATION_DIRECTORY = "config"

LICENSE = "Apache-2.0 | BSD-2-Clause | BSD-3-Clause | MIT | PD | Proprietary"

SRC_URI_arm = " \
    http:///not/exist/greengrass-linux-armv7l-${PV}.tar.gz;name=arm \
    file://greengrass.service \
    file://greengrass-init \
"

SRC_URI_aarch64 = " \
    http:///not/exist/greengrass-linux-aarch64-${PV}.tar.gz;name=aarch64 \
    file://greengrass.service \
    file://greengrass-init \
"

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

inherit aws-iot update-rc.d useradd systemd

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
	install -m 0755 ${WORKDIR}/greengrass-init ${D}${sysconfdir}/greengrass
	sed -i -e "s,##GG_INSTALL_DIR##,/${BPN},g" ${D}${sysconfdir}/greengrass
	ln -sf ${sysconfdir}/greengrass ${D}${sysconfdir}/init.d/greengrass

	# Install systemd service
	install -d ${D}${systemd_unitdir}/system/
	install -m 0644 ${WORKDIR}/greengrass.service ${D}${systemd_unitdir}/system/greengrass.service

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

pkg_postinst_ontarget_${PN}() {
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

FILES_${PN} = "/${BPN} ${sysconfdir} ${systemd_unitdir}"

CONFFILES_${PN} += "/${BPN}/config/config.json"

INITSCRIPT_NAME = "greengrass"
INITSCRIPT_PARAMS = "defaults 80 20"

SYSTEMD_SERVICE_${PN} = "greengrass.service"

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

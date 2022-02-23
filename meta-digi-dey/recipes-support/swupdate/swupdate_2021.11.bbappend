# Copyright (C) 2016-2022 Digi International Inc.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

# Without libgcc, swupdate generates an error signal when terminating
RDEPENDS_${PN} += "libgcc"

do_configure_append() {
	# If Trustfence is enabled, enable the signing support in the
	# '.config' file.
	if [ "${TRUSTFENCE_SIGN}" = "1" ]; then
		echo "CONFIG_SIGNED_IMAGES=y" >> ${B}/.config
	fi
	cml1_do_configure
}

do_install_append() {
	# Copy the 'progress' binary.
	install -d ${D}${bindir}/
	install -m 0755 tools/swupdate-progress ${D}${bindir}/progress
}

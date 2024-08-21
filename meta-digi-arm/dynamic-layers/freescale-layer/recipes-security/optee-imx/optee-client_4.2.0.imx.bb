# Copyright (C) 2024, Digi International Inc.

#
# Reuse meta-freescale's optee-client_3.19.0.imx.bb
#
require recipes-security/optee-imx/optee-client_3.19.0.imx.bb

SRC_URI += "${@oe.utils.vartrue('TRUSTFENCE_FILE_BASED_ENCRYPT', 'file://tee-supplicant', '', d)}"
SRCBRANCH = "lf-6.6.23_2.0.0"
SRCREV = "3eac340a781c00ccd61b151b0e9c22a8c6e9f9f0"

EXTRA_OEMAKE += "PKG_CONFIG=pkg-config CFG_TEE_FS_PARENT_PATH='${localstatedir}/lib/tee'"

do_install() {
	oe_runmake DESTDIR=${D} install
	install -D -p -m0644 ${WORKDIR}/tee-supplicant.service ${D}${systemd_system_unitdir}/tee-supplicant.service
	sed -i -e s:@sysconfdir@:${sysconfdir}:g \
		-e s:@sbindir@:${sbindir}:g \
		${D}${systemd_system_unitdir}/tee-supplicant.service

	if ${@oe.utils.vartrue('TRUSTFENCE_FILE_BASED_ENCRYPT', 'true', 'false',d)}; then
		install -d ${D}${sysconfdir}/default/
		install -m 0644 ${WORKDIR}/tee-supplicant ${D}${sysconfdir}/default/tee-supplicant
	fi
}

COMPATIBLE_MACHINE = "(ccimx91)"

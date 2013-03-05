DESCRIPTION = "Atheros's wireless driver"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://Ath6kl_LinuxRelease/Generic_Packages/compat-wireless/COPYRIGHT;md5=d7810fab7487fb0aad327b76f1be7cd7"

inherit module

PR = "r0"

ATHEROS_BUILD_SRC ?= "1"

SRCREV = "${AUTOREV}"
SRC_URI_git = " \
	${DIGI_LOG_GIT}linux-modules/atheros.git;protocol=git;branch=refs/heads/master \
	file://Makefile.git \
	"
SRC_URI_obj = " \
	file://atheros-${MACHINE}.tar.gz \
	file://Makefile.obj \
	"

SRC_URI  = "${@base_conditional('ATHEROS_BUILD_SRC', '1' , '${SRC_URI_git}', '${SRC_URI_obj}', d)}"
SRC_URI += " \
	file://atheros \
	file://atheros.conf \
	"

S = "${@base_conditional('ATHEROS_BUILD_SRC', '1' , '${WORKDIR}/git', '${WORKDIR}/${MACHINE}', d)}"

EXTRA_OEMAKE = "DEL_PLATFORM=${MACHINE} KLIB_BUILD=${STAGING_KERNEL_DIR}"

do_configure_prepend() {
	[ "${ATHEROS_BUILD_SRC}" = "1" ] && MK_SUFFIX=".git" || MK_SUFFIX=".obj"
	cp ${WORKDIR}/Makefile${MK_SUFFIX} ${S}/Makefile
}

do_install_append() {
	install -d ${D}${sysconfdir}/network/if-pre-up.d
	install -m 0755 ${WORKDIR}/atheros ${D}${sysconfdir}/network/if-pre-up.d/
	install -d ${D}${sysconfdir}/modprobe.d
	install -m 0644 ${WORKDIR}/atheros.conf ${D}${sysconfdir}/modprobe.d/
}

FILES_${PN} += " \
	/lib/firmware/ath6k/AR6003/hw2.1.1/athtcmd_ram.bin \
	/lib/firmware/ath6k/AR6003/hw2.1.1/athwlan.bin \
	/lib/firmware/ath6k/AR6003/hw2.1.1/Digi_6203-6233-US.bin \
	/lib/firmware/ath6k/AR6003/hw2.1.1/Digi_6203-6233-World.bin \
	/lib/firmware/ath6k/AR6003/hw2.1.1/fw-4.bin \
	/lib/firmware/ath6k/AR6003/hw2.1.1/nullTestFlow.bin \
	/lib/firmware/ath6k/AR6003/hw2.1.1/utf.bin \
	"

# Deploy objects tarball if building from sources
do_deploy() {
	if [ "${ATHEROS_BUILD_SRC}" = "1" ]; then
		oe_runmake tarball
		install -d ${DEPLOY_DIR_IMAGE}
		cp ${S}/atheros-${MACHINE}.tar.gz ${DEPLOY_DIR_IMAGE}/
	fi
}

addtask deploy before do_build after do_install

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(ccardimx28js|cpx2)"

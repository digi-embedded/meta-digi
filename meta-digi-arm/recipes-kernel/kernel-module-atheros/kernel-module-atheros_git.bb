DESCRIPTION = "Atheros's wireless driver"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://Ath6kl_LinuxRelease/Generic_Packages/compat-wireless/COPYRIGHT;md5=d7810fab7487fb0aad327b76f1be7cd7"

inherit module

PR = "r0"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
SRCREV = "master"
SRC_URI = "${DIGI_LOG_GIT}linux-modules/atheros.git;protocol=git \
	   file://atheros \
	  "

S = "${WORKDIR}/git"
FIRMWARE_S = "${S}/Ath6kl_LinuxRelease/Firmware_Package/target/AR6003/hw2.1.1"

EXTRA_OEMAKE = "-C ${STAGING_KERNEL_DIR}"
EXTRAMAKEFLAGS = "M=${S} CONFIG_DEL_KMOD_ATHEROS=y KLIB_BUILD=${STAGING_KERNEL_DIR}"

module_do_compile() {
        unset CFLAGS CPPFLAGS CXXFLAGS LDFLAGS
        oe_runmake KERNEL_PATH=${STAGING_KERNEL_DIR}   \
                   KERNEL_SRC=${STAGING_KERNEL_DIR}    \
                   CC="${KERNEL_CC}" LD="${KERNEL_LD}" \
                   AR="${KERNEL_AR}" \
		   ${EXTRAMAKEFLAGS} \
                   ${MAKE_TARGETS}
}

module_do_install() {
        unset CFLAGS CPPFLAGS CXXFLAGS LDFLAGS
        oe_runmake DEPMOD=echo INSTALL_MOD_PATH="${D}" \
                   KERNEL_SRC=${STAGING_KERNEL_DIR} \
                   CC="${KERNEL_CC}" LD="${KERNEL_LD}" \
		   ${EXTRAMAKEFLAGS} \
                   modules_install
}

do_install_append() {
        install -d ${D}${sysconfdir}/network/if-pre-up.d
	install -m 0755 ${WORKDIR}/atheros  ${D}${sysconfdir}/network/if-pre-up.d/
	install -d ${D}/lib/firmware/ath6k/AR6003/hw2.1.1
	install -m 0755 ${FIRMWARE_S}/ath6kl_fw_concurrency/athtcmd_ram.bin ${D}/lib/firmware/ath6k/AR6003/hw2.1.1/
	install -m 0755 ${FIRMWARE_S}/ath6kl_fw_concurrency/athwlan.bin ${D}/lib/firmware/ath6k/AR6003/hw2.1.1/
	install -m 0755 ${FIRMWARE_S}/ath6kl_fw_concurrency/fw-4.bin ${D}/lib/firmware/ath6k/AR6003/hw2.1.1/
	install -m 0755 ${FIRMWARE_S}/ath6kl_fw_concurrency/nullTestFlow.bin ${D}/lib/firmware/ath6k/AR6003/hw2.1.1/
	install -m 0755 ${FIRMWARE_S}/ath6kl_fw_concurrency/utf.bin ${D}/lib/firmware/ath6k/AR6003/hw2.1.1/
	install -m 0755 ${FIRMWARE_S}/Digi_6203-6233-US.bin ${D}/lib/firmware/ath6k/AR6003/hw2.1.1/
	install -m 0755 ${FIRMWARE_S}/Digi_6203-6233-World.bin ${D}/lib/firmware/ath6k/AR6003/hw2.1.1/
}

FILES_${PN} += " /lib/firmware/ath6k/AR6003/hw2.1.1/athtcmd_ram.bin \
		/lib/firmware/ath6k/AR6003/hw2.1.1/athwlan.bin \
		/lib/firmware/ath6k/AR6003/hw2.1.1/fw-4.bin \
		/lib/firmware/ath6k/AR6003/hw2.1.1/nullTestFlow.bin \
		/lib/firmware/ath6k/AR6003/hw2.1.1/utf.bin \
		/lib/firmware/ath6k/AR6003/hw2.1.1/Digi_6203-6233-US.bin \
		/lib/firmware/ath6k/AR6003/hw2.1.1/Digi_6203-6233-World.bin "

COMPATIBLE_MACHINE = "(ccardimx28js)"

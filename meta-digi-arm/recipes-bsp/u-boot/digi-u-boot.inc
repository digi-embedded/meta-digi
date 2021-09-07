# Copyright (C) 2018 Digi International

require recipes-bsp/u-boot/u-boot.inc

DESCRIPTION = "Bootloader for Digi platforms"
LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://Licenses/README;md5=a2c678cfd4a4d97135585cad908541c6"
SECTION = "bootloaders"

DEPENDS += "bc-native dtc-native u-boot-mkimage-native"
DEPENDS += "${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', 'trustfence-sign-tools-native', '', d)}"

PROVIDES += "u-boot"

S = "${WORKDIR}/git"

# Select internal or Github U-Boot repo
DIGI_LOG_REPO = "u-boot-denx.git"
DIGI_MTK_REPO = "uboot/u-boot-denx.git"
GITHUB_REPO = "u-boot.git"
UBOOT_GIT_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1' , \
					oe.utils.conditional('DIGI_GIT', '${DIGI_LOG_GIT}', '${DIGI_GIT}/${DIGI_LOG_REPO}', '${DIGI_GIT}/${DIGI_MTK_REPO};protocol=https', d), \
					'${DIGI_GITHUB_GIT}/${GITHUB_REPO};protocol=https', d)}"

SRC_URI = " \
    ${UBOOT_GIT_URI};branch=${SRCBRANCH} \
"

SRC_URI_append = " \
    file://boot.txt \
    file://install_linux_fw_sd.txt \
    file://install_linux_fw_usb.txt \
    ${@oe.utils.conditional('UBOOT_HAS_FASTBOOT', 'true', 'file://install_linux_fw_uuu.sh', '', d)} \
"

BOOTLOADER_IMAGE_RECIPE ?= "u-boot"

LOCALVERSION ?= ""
inherit fsl-u-boot-localversion

EXTRA_OEMAKE_append = " KCFLAGS=-fgnu89-inline"

UBOOT_EXTRA_CONF ?= ""

# Use default initial env name so userspace tools work as expected
UBOOT_INITIAL_ENV = "u-boot-initial-env"

python __anonymous() {
    if (d.getVar("TRUSTFENCE_DEK_PATH", True) not in ["0", None]) and (d.getVar("TRUSTFENCE_SIGN", True) != "1"):
        bb.fatal("Only signed U-Boot images can be encrypted. Generate signed images (TRUSTFENCE_SIGN = \"1\") or remove encryption (TRUSTFENCE_DEK_PATH = \"0\")")
}

do_compile () {
	if [ "${@bb.utils.filter('DISTRO_FEATURES', 'ld-is-gold', d)}" ]; then
		sed -i 's/$(CROSS_COMPILE)ld$/$(CROSS_COMPILE)ld.bfd/g' ${S}/config.mk
	fi

	unset LDFLAGS
	unset CFLAGS
	unset CPPFLAGS

	if [ ! -e ${B}/.scmversion -a ! -e ${S}/.scmversion ]
	then
		echo ${UBOOT_LOCALVERSION} > ${B}/.scmversion
		echo ${UBOOT_LOCALVERSION} > ${S}/.scmversion
	fi

	if [ -n "${UBOOT_CONFIG}" ]
	then
		unset i j k
		for config in ${UBOOT_MACHINE}; do
			i=$(expr $i + 1);
			for type in ${UBOOT_CONFIG}; do
				j=$(expr $j + 1);
				if [ $j -eq $i ]
				then
					oe_runmake -C ${S} O=${B}/${config} ${config}
					# Reconfigure U-Boot with Digi UBOOT_EXTRA_CONF
					for var in ${UBOOT_EXTRA_CONF}; do
						echo "${var}" >> ${B}/${config}/.config
					done
					oe_runmake -C ${S} O=${B}/${config} oldconfig

					oe_runmake -C ${S} O=${B}/${config} ${UBOOT_MAKE_TARGET}
					for binary in ${UBOOT_BINARIES}; do
						k=$(expr $k + 1);
						if [ $k -eq $i ]; then
							cp ${B}/${config}/${binary} ${B}/${config}/u-boot-${type}.${UBOOT_SUFFIX}
						fi
					done

					# Generate the uboot-initial-env
					if [ -n "${UBOOT_INITIAL_ENV}" ]; then
						oe_runmake -C ${S} O=${B}/${config} u-boot-initial-env
						cp ${B}/${config}/u-boot-initial-env ${B}/${config}/u-boot-initial-env-${type}
					fi

					unset k

					# Secure boot artifacts
					if [ "${TRUSTFENCE_SIGN}" = "1" ] && [ "${BOOTLOADER_IMAGE_RECIPE}" = "u-boot" ]
					then
						cp ${B}/${config}/u-boot-dtb-signed.imx ${B}/${config}/u-boot-dtb-signed-${type}.${UBOOT_SUFFIX}
						cp ${B}/${config}/u-boot-dtb-usb-signed.imx ${B}/${config}/u-boot-dtb-usb-signed-${type}.${UBOOT_SUFFIX}
						if [ "${TRUSTFENCE_DEK_PATH}" != "0" ]
						then
							cp ${B}/${config}/u-boot-dtb-encrypted.imx ${B}/${config}/u-boot-dtb-encrypted-${type}.${UBOOT_SUFFIX}
						fi
					fi
				fi
			done
			unset  j
		done
		unset  i
	else
		oe_runmake -C ${S} O=${B} ${UBOOT_MACHINE}
		oe_runmake -C ${S} O=${B} ${UBOOT_MAKE_TARGET}

		# Generate the uboot-initial-env
		if [ -n "${UBOOT_INITIAL_ENV}" ]; then
			oe_runmake -C ${S} O=${B} u-boot-initial-env
		fi
	fi
}

TF_BOOTSCRIPT_SEDFILTER = "${@tf_bootscript_sedfilter(d)}"

def tf_bootscript_sedfilter(d):
    tf_initramfs = d.getVar('TRUSTFENCE_INITRAMFS_IMAGE',True) or ""
    return "s,\(^[[:blank:]]*\)true.*,\\1setenv boot_initrd true\\n\\1setenv initrd_file %s-${MACHINE}.cpio.gz.u-boot.tf,g" % tf_initramfs if tf_initramfs else ""

do_deploy_append() {
	# Remove canonical U-Boot symlinks for ${UBOOT_CONFIG} currently in the form:
	#    u-boot-<platform>.<ext>-<type>
	#    u-boot-<type>
	# and add a more suitable symlink in the form:
	#    u-boot-<platform>-<config>.<ext>
	if [ -n "${UBOOT_CONFIG}" ]
	then
		for config in ${UBOOT_MACHINE}; do
			i=$(expr $i + 1);
			for type in ${UBOOT_CONFIG}; do
				j=$(expr $j + 1);
				if [ $j -eq $i ]
				then
					cd ${DEPLOYDIR}
					rm -r ${UBOOT_BINARY}-${type}
					ln -sf u-boot-${type}-${PV}-${PR}.${UBOOT_SUFFIX} u-boot-${type}.${UBOOT_SUFFIX}
					if [ "${TRUSTFENCE_SIGN}" = "1" ] && [ "${BOOTLOADER_IMAGE_RECIPE}" = "u-boot" ]; then
						install ${B}/${config}/SRK_efuses.bin SRK_efuses-${PV}-${PR}.bin
						ln -sf SRK_efuses-${PV}-${PR}.bin SRK_efuses.bin

						install ${B}/${config}/u-boot-dtb-signed-${type}.${UBOOT_SUFFIX} u-boot-dtb-signed-${type}-${PV}-${PR}.${UBOOT_SUFFIX}
						ln -sf u-boot-dtb-signed-${type}-${PV}-${PR}.${UBOOT_SUFFIX} u-boot-dtb-signed-${type}.${UBOOT_SUFFIX}

						install ${B}/${config}/u-boot-dtb-usb-signed-${type}.${UBOOT_SUFFIX} u-boot-dtb-usb-signed-${type}-${PV}-${PR}.${UBOOT_SUFFIX}
						ln -sf u-boot-dtb-usb-signed-${type}-${PV}-${PR}.${UBOOT_SUFFIX} u-boot-dtb-usb-signed-${type}.${UBOOT_SUFFIX}

						if [ "${TRUSTFENCE_DEK_PATH}" != "0" ]
						then
							install ${B}/${config}/u-boot-dtb-encrypted-${type}.${UBOOT_SUFFIX} u-boot-dtb-encrypted-${type}-${PV}-${PR}.${UBOOT_SUFFIX}
							ln -sf u-boot-dtb-encrypted-${type}-${PV}-${PR}.${UBOOT_SUFFIX} u-boot-dtb-encrypted-${type}.${UBOOT_SUFFIX}
						fi
					fi
				fi
			done
			unset  j
		done
		unset  i
	fi

	# DEY firmware install scripts
	sed -i -e 's,##GRAPHICAL_BACKEND##,${GRAPHICAL_BACKEND},g' ${WORKDIR}/install_linux_fw_sd.txt ${WORKDIR}/install_linux_fw_usb.txt
	sed -i -e 's,##MACHINE##,${MACHINE},g' ${WORKDIR}/install_linux_fw_sd.txt  ${WORKDIR}/install_linux_fw_usb.txt
	sed -i -e 's,##GRAPHICAL_IMAGES##,${GRAPHICAL_IMAGES},g' ${WORKDIR}/install_linux_fw_sd.txt  ${WORKDIR}/install_linux_fw_usb.txt
	sed -i -e 's,##DEFAULT_IMAGE_NAME##,${DEFAULT_IMAGE_NAME},g' ${WORKDIR}/install_linux_fw_sd.txt  ${WORKDIR}/install_linux_fw_usb.txt
	mkimage -T script -n "DEY firmware install script" -C none -d ${WORKDIR}/install_linux_fw_sd.txt ${DEPLOYDIR}/install_linux_fw_sd.scr
	mkimage -T script -n "DEY firmware install script" -C none -d ${WORKDIR}/install_linux_fw_usb.txt ${DEPLOYDIR}/install_linux_fw_usb.scr

	if [ "${@oe.utils.conditional('UBOOT_HAS_FASTBOOT', 'true', 'true', '', d)}" = "true" ]; then
		# DEY firmware install uuu script
		sed -i -e 's,##GRAPHICAL_BACKEND##,${GRAPHICAL_BACKEND},g' ${WORKDIR}/install_linux_fw_uuu.sh
		sed -i -e 's,##MACHINE##,${MACHINE},g' ${WORKDIR}/install_linux_fw_uuu.sh
		sed -i -e 's,##GRAPHICAL_IMAGES##,${GRAPHICAL_IMAGES},g' ${WORKDIR}/install_linux_fw_uuu.sh
		sed -i -e 's,##DEFAULT_IMAGE_NAME##,${DEFAULT_IMAGE_NAME},g' ${WORKDIR}/install_linux_fw_uuu.sh
		install -m 775 ${WORKDIR}/install_linux_fw_uuu.sh ${DEPLOYDIR}/
	fi

	# Boot script for DEY images (reconfigure on-the-fly if TRUSTFENCE is enabled)
	TMP_BOOTSCR="$(mktemp ${WORKDIR}/bootscr.XXXXXX)"
	sed -e "${TF_BOOTSCRIPT_SEDFILTER}" ${WORKDIR}/boot.txt > ${TMP_BOOTSCR}
	mkimage -T script -n bootscript -C none -d ${TMP_BOOTSCR} ${DEPLOYDIR}/boot.scr

	# Sign the scripts
	if [ "${TRUSTFENCE_SIGN}" = "1" ]; then
		export CONFIG_SIGN_KEYS_PATH="${TRUSTFENCE_SIGN_KEYS_PATH}"
		[ -n "${TRUSTFENCE_KEY_INDEX}" ] && export CONFIG_KEY_INDEX="${TRUSTFENCE_KEY_INDEX}"
		[ -n "${TRUSTFENCE_DEK_PATH}" ] && [ "${TRUSTFENCE_DEK_PATH}" != "0" ] && export CONFIG_DEK_PATH="${TRUSTFENCE_DEK_PATH}"
		[ -n "${TRUSTFENCE_SIGN_MODE}" ] && export CONFIG_SIGN_MODE="${TRUSTFENCE_SIGN_MODE}"

		# Sign boot script
		TMP_SIGNED_BOOTSCR="$(mktemp ${WORKDIR}/bootscr-signed.XXXXXX)"
		trustfence-sign-artifact.sh -p "${DIGI_FAMILY}" -b "${DEPLOYDIR}/boot.scr" "${TMP_SIGNED_BOOTSCR}"
		mv "${TMP_SIGNED_BOOTSCR}" "${DEPLOYDIR}/boot.scr"
	fi
	rm -f ${TMP_BOOTSCR}
}

BOOT_TOOLS = "imx-boot-tools"

do_deploy_append_ccimx8x() {
	# Move all U-Boot artifacts to the imx-boot-tools folder
	# U-Boot images are not bootable on the i.MX8X
	install -d ${DEPLOYDIR}/${BOOT_TOOLS}
	mv ${DEPLOYDIR}/u-boot* ${DEPLOYDIR}/${BOOT_TOOLS}/
	mv ${DEPLOYDIR}/${UBOOT_SYMLINK}-* ${DEPLOYDIR}/${BOOT_TOOLS}/
}

do_deploy_append_ccimx8m() {
	# Deploy u-boot-nodtb.bin and ccimx8m[m|n]-dvk.dtb, to be packaged in boot binary by imx-boot
	if [ -n "${UBOOT_CONFIG}" ]
	then
		for config in ${UBOOT_MACHINE}; do
			i=$(expr $i + 1);
			for type in ${UBOOT_CONFIG}; do
				j=$(expr $j + 1);
				if [ $j -eq $i ]
				then
					install -d ${DEPLOYDIR}/${BOOT_TOOLS}
					install -m 0777 ${B}/${config}/arch/arm/dts/${UBOOT_DTB_NAME}  ${DEPLOYDIR}/${BOOT_TOOLS}
					install -m 0777 ${B}/${config}/u-boot-nodtb.bin  ${DEPLOYDIR}/${BOOT_TOOLS}/u-boot-nodtb.bin-${MACHINE}-${UBOOT_CONFIG}
				fi
			done
			unset  j
		done
		unset  i
	fi
}

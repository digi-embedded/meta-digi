# Copyright (C) 2018-2022 Digi International

require recipes-bsp/u-boot/u-boot.inc

DESCRIPTION = "Bootloader for Digi platforms"
LICENSE = "GPL-2.0-or-later"
LIC_FILES_CHKSUM = "file://Licenses/README;md5=a2c678cfd4a4d97135585cad908541c6"
SECTION = "bootloaders"

DEPENDS += "bc-native dtc-native u-boot-mkimage-native"
DEPENDS += "${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', 'trustfence-sign-tools-native', '', d)}"

PROVIDES += "u-boot"

S = "${WORKDIR}/git"
B = "${WORKDIR}/build"

# Select internal or Github U-Boot repo
UBOOT_URI_STASH = "${DIGI_MTK_GIT}/uboot/u-boot-denx.git;protocol=ssh"
UBOOT_URI_GITHUB = "${DIGI_GITHUB_GIT}/u-boot.git;protocol=https"
UBOOT_GIT_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1' , '${UBOOT_URI_STASH}', '${UBOOT_URI_GITHUB}', d)}"

INSTALL_FW_UBOOT_SCRIPTS = " \
    file://install_linux_fw_sd.txt \
    file://install_linux_fw_usb.txt \
    file://install_linux_fw_uuu.sh \
"

SRC_URI = " \
    ${UBOOT_GIT_URI};branch=${SRCBRANCH} \
    file://altboot.txt \
    file://boot.txt \
    ${INSTALL_FW_UBOOT_SCRIPTS} \
"

BUILD_UBOOT_SCRIPTS ?= "true"

BOOTLOADER_IMAGE_RECIPE ?= "u-boot"

LOCALVERSION ?= ""
inherit ${@oe.utils.conditional('DEY_BUILD_PLATFORM', 'NXP', 'fsl-u-boot-localversion', '', d)}

# Disable u-boot environment artifacts
UBOOT_INITIAL_ENV = ""

python __anonymous() {
    if (d.getVar("TRUSTFENCE_DEK_PATH") not in ["0", None]) and (d.getVar("TRUSTFENCE_SIGN") != "1"):
        bb.fatal("Only signed U-Boot images can be encrypted. Generate signed images (TRUSTFENCE_SIGN = \"1\") or remove encryption (TRUSTFENCE_DEK_PATH = \"0\")")
}

do_configure[prefuncs] += "${@oe.utils.ifelse(d.getVar('UBOOT_TF_CONF'), 'trustfence_config', '')}"
python trustfence_config() {
    config_path = d.expand('${WORKDIR}/uboot-trustfence.cfg')
    with open(config_path, 'w') as f:
        for cfg in d.getVar('UBOOT_TF_CONF').split():
            f.write('%s\n' % cfg)
    d.appendVar('SRC_URI', ' file://%s' % config_path)
}

TF_BOOTSCRIPT_SEDFILTER = "${@tf_bootscript_sedfilter(d)}"

def tf_bootscript_sedfilter(d):
    tf_initramfs = d.getVar('TRUSTFENCE_INITRAMFS_IMAGE') or ""
    return "s,\(^[[:blank:]]*\)true.*,\\1setenv boot_initrd true\\n\\1setenv initrd_file %s-${MACHINE}.cpio.gz.u-boot.tf,g" % tf_initramfs if tf_initramfs else ""

build_uboot_scripts() {
	for f in $(echo ${INSTALL_FW_UBOOT_SCRIPTS} | sed -e 's,file\:\/\/,,g'); do
		f_ext="${f##*.}"
		TMP_INSTALL_SCR="$(mktemp ${WORKDIR}/${f}.XXXXXX)"
		sed -e 's,##GRAPHICAL_BACKEND##,${GRAPHICAL_BACKEND},g' \
			-e 's,##MACHINE##,${MACHINE},g' \
			-e 's,##GRAPHICAL_IMAGES##,${GRAPHICAL_IMAGES},g' \
			-e 's,##DEFAULT_IMAGE_NAME##,${DEFAULT_IMAGE_NAME},g' \
			${WORKDIR}/${f} > ${TMP_INSTALL_SCR}
		if [ "${f_ext}" = "txt" ]; then
			mkimage -T script -n "DEY firmware install script" -C none -d ${TMP_INSTALL_SCR} ${DEPLOYDIR}/${f%.*}.scr
		else
			install -m 775 ${TMP_INSTALL_SCR} ${DEPLOYDIR}/${f}
		fi
		rm -f ${TMP_INSTALL_SCR}
	done

	# Boot script for DEY images (reconfigure on-the-fly if TRUSTFENCE is enabled)
	TMP_BOOTSCR="$(mktemp ${WORKDIR}/bootscr.XXXXXX)"
	sed -e "${TF_BOOTSCRIPT_SEDFILTER}" ${WORKDIR}/boot.txt > ${TMP_BOOTSCR}
	mkimage -T script -n bootscript -C none -d ${TMP_BOOTSCR} ${DEPLOYDIR}/boot.scr
	rm -f ${TMP_BOOTSCR}

	# Alternate boot script for dualboot
	mkimage -T script -n "Alternate bootscript" -C none -d ${WORKDIR}/altboot.txt ${DEPLOYDIR}/altboot.scr

	# Sign the scripts
	if [ "${TRUSTFENCE_SIGN}" = "1" ]; then
		export CONFIG_SIGN_KEYS_PATH="${TRUSTFENCE_SIGN_KEYS_PATH}"
		[ -n "${TRUSTFENCE_KEY_INDEX}" ] && export CONFIG_KEY_INDEX="${TRUSTFENCE_KEY_INDEX}"
		[ -n "${TRUSTFENCE_DEK_PATH}" ] && [ "${TRUSTFENCE_DEK_PATH}" != "0" ] && export CONFIG_DEK_PATH="${TRUSTFENCE_DEK_PATH}"

		# Sign boot script
		TMP_SIGNED_BOOTSCR="$(mktemp ${WORKDIR}/bootscr-signed.XXXXXX)"
		trustfence-sign-artifact.sh -p "${DIGI_FAMILY}" -b "${DEPLOYDIR}/boot.scr" "${TMP_SIGNED_BOOTSCR}"
		mv "${TMP_SIGNED_BOOTSCR}" "${DEPLOYDIR}/boot.scr"

		# Sign altboot script
		TMP_SIGNED_BOOTSCR="$(mktemp ${WORKDIR}/altboot-signed.XXXXXX)"
		trustfence-sign-artifact.sh -p "${DIGI_FAMILY}" -b "${DEPLOYDIR}/altboot.scr" "${TMP_SIGNED_BOOTSCR}"
		mv "${TMP_SIGNED_BOOTSCR}" "${DEPLOYDIR}/altboot.scr"
	fi
}

do_deploy:append() {
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
					ln -sf ${UBOOT_BINARYNAME}-${type}-${PV}-${PR}.${UBOOT_SUFFIX} ${UBOOT_BINARYNAME}-${type}.${UBOOT_SUFFIX}
					if [ "${TRUSTFENCE_SIGN}" = "1" ] && [ "${BOOTLOADER_IMAGE_RECIPE}" = "u-boot" ]; then
						install ${B}/${config}/SRK_efuses.bin SRK_efuses-${PV}-${PR}.bin
						ln -sf SRK_efuses-${PV}-${PR}.bin SRK_efuses.bin

						install ${B}/${config}/${UBOOT_BINARYNAME}-dtb-signed.imx ${UBOOT_BINARYNAME}-dtb-signed-${type}-${PV}-${PR}.${UBOOT_SUFFIX}
						ln -sf ${UBOOT_BINARYNAME}-dtb-signed-${type}-${PV}-${PR}.${UBOOT_SUFFIX} ${UBOOT_BINARYNAME}-dtb-signed-${type}.${UBOOT_SUFFIX}

						install ${B}/${config}/${UBOOT_BINARYNAME}-dtb-usb-signed.imx ${UBOOT_BINARYNAME}-dtb-usb-signed-${type}-${PV}-${PR}.${UBOOT_SUFFIX}
						ln -sf ${UBOOT_BINARYNAME}-dtb-usb-signed-${type}-${PV}-${PR}.${UBOOT_SUFFIX} ${UBOOT_BINARYNAME}-dtb-usb-signed-${type}.${UBOOT_SUFFIX}

						if [ "${TRUSTFENCE_DEK_PATH}" != "0" ]
						then
							install ${B}/${config}/${UBOOT_BINARYNAME}-dtb-encrypted.imx ${UBOOT_BINARYNAME}-dtb-encrypted-${type}-${PV}-${PR}.${UBOOT_SUFFIX}
							ln -sf ${UBOOT_BINARYNAME}-dtb-encrypted-${type}-${PV}-${PR}.${UBOOT_SUFFIX} ${UBOOT_BINARYNAME}-dtb-encrypted-${type}.${UBOOT_SUFFIX}
						fi
					fi
				fi
			done
			unset  j
		done
		unset  i
	fi

	if [ "${BUILD_UBOOT_SCRIPTS}" = "true" ]; then
		build_uboot_scripts
	fi
}

BOOT_TOOLS = "imx-boot-tools"
BOOT_TOOLS:ccmp1 = "u-boot"
FIP_UBOOT_HEADER = "ccmp15-dvk"
FIP_UBOOT_HEADER:ccmp13 = "ccmp13-dvk"

do_deploy:append:ccimx8x() {
	# Move all U-Boot artifacts to the imx-boot-tools folder
	# U-Boot images are not bootable on the i.MX8X
	install -d ${DEPLOYDIR}/${BOOT_TOOLS}
	mv ${DEPLOYDIR}/u-boot* ${DEPLOYDIR}/${BOOT_TOOLS}/
	mv ${DEPLOYDIR}/${UBOOT_SYMLINK}-* ${DEPLOYDIR}/${BOOT_TOOLS}/
}

do_deploy:append:ccimx8m() {
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

do_deploy:append:ccmp1() {
	# Deploy u-boot-nodtb.bin and ccmp1x-dvk.dtb, to be packaged in fip binary by tf-a
	install -d ${DEPLOYDIR}/${BOOT_TOOLS}
	install -m 0777 ${B}/${config}/arch/arm/dts/${UBOOT_DTB_NAME} ${DEPLOYDIR}/${BOOT_TOOLS}/${FIP_UBOOT_DTB}-${FIP_UBOOT_HEADER}.dtb

	install -m 0777 ${B}/${config}/u-boot-nodtb.bin  ${DEPLOYDIR}/${BOOT_TOOLS}/u-boot-nodtb.bin
}

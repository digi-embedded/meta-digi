# Copyright (C) 2018-2025, Digi International Inc.

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
    file://boot.txt \
    file://install_linux_fw_media.txt \
    file://install_linux_fw_uuu.sh \
"

BUILD_UBOOT_SCRIPTS ?= "true"

BOOTLOADER_IMAGE_RECIPE ?= "u-boot"

LOCALVERSION ?= ""
inherit ${@oe.utils.conditional('DEY_SOC_VENDOR', 'NXP', 'fsl-u-boot-localversion', '', d)}

# Disable u-boot environment artifacts
UBOOT_INITIAL_ENV = ""

python __anonymous() {
    if (d.getVar("TRUSTFENCE_DEK_PATH") not in ["0", None]) and (d.getVar("TRUSTFENCE_SIGN") != "1"):
        bb.fatal("Only signed U-Boot images can be encrypted. Generate signed images (TRUSTFENCE_SIGN = \"1\") or remove encryption (TRUSTFENCE_DEK_PATH = \"0\")")
}

do_configure[prefuncs] += "${@oe.utils.ifelse(d.getVar('UBOOT_TF_CONF'), 'trustfence_config', '')}"
python trustfence_config() {
    import shlex
    config_path = d.expand('${WORKDIR}/uboot-trustfence.cfg')
    with open(config_path, 'w') as f:
        for cfg in shlex.split(d.getVar('UBOOT_TF_CONF'), posix=False):
            # strip quotes for "is not set" options
            if 'is not set' in cfg:
                cfg = cfg.strip('"\'')
            f.write('%s\n' % cfg)
    d.appendVar('SRC_URI', ' file://%s' % config_path)
}

TF_BOOTSCRIPT_SEDFILTER = "${@tf_bootscript_sedfilter(d)}"

def tf_bootscript_sedfilter(d):
    tf_initramfs = d.getVar('TRUSTFENCE_INITRAMFS_IMAGE') or ""
    return "s,\(^[[:blank:]]*\)true.*,\\1setenv boot_initrd true\\n\\1setenv initrd_file %s-${MACHINE}.cpio.gz.u-boot.tf,g" % tf_initramfs if tf_initramfs else ""

SIGN_UBOOT ?= ""
SIGN_UBOOT:ccimx6 = "sign_uboot"
SIGN_UBOOT:ccimx6ul = "sign_uboot"

do_deploy[postfuncs] += " \
    ${@oe.utils.ifelse(d.getVar('BUILD_UBOOT_SCRIPTS') == 'true', 'build_uboot_scripts', '')} \
    ${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', '${SIGN_UBOOT}', '', d)} \
"

build_uboot_scripts() {
	# Generate USB installer from media template
	sed -e 's,##INSTALL_MEDIA##,usb,g' \
	    -e 's,##INSTALL_MEDIA_INDEX##,${UBOOT_INSTALL_USB_INDEX},g' \
	    ${WORKDIR}/install_linux_fw_media.txt > ${WORKDIR}/install_linux_fw_usb.txt
	# Generate microSD installer from media template
	sed -e 's,##INSTALL_MEDIA##,mmc,g' \
	    -e 's,##INSTALL_MEDIA_INDEX##,${UBOOT_INSTALL_SD_INDEX},g' \
	    ${WORKDIR}/install_linux_fw_media.txt > ${WORKDIR}/install_linux_fw_sd.txt

	for f in $(echo ${INSTALL_FW_UBOOT_SCRIPTS} | sed -e 's,file\:\/\/,,g'); do
		f_ext="${f##*.}"
		TMP_INSTALL_SCR="$(mktemp ${WORKDIR}/${f}.XXXXXX)"
		sed -e 's,##GRAPHICAL_BACKEND##,${GRAPHICAL_BACKEND},g' \
			-e 's,##MACHINE##,${MACHINE},g' \
			-e 's,##GRAPHICAL_IMAGES##,${GRAPHICAL_IMAGES},g' \
			-e 's,##DEFAULT_IMAGE_NAME##,${DEFAULT_IMAGE_NAME},g' \
			${WORKDIR}/${f} > ${TMP_INSTALL_SCR}
		if [ "${DEY_SOC_VENDOR}" = "STM" ]; then
			sed -i -e 's,##BOOTSCHEME_DEFAULT##,${BOOTSCHEME_DEFAULT},g' ${TMP_INSTALL_SCR}
		fi
		# Change the u-boot name when TrustFence is enabled
		if [ "${TRUSTFENCE_SIGN}" = "1" ]; then
			if [ "${DEY_SOC_VENDOR}" = "NXP" ]; then
				if [ "${TRUSTFENCE_DEK_PATH}" != "0" ]; then
					sed -i -e 's,##SIGNED##,encrypted,g' ${TMP_INSTALL_SCR}
				else
					sed -i -e 's,##SIGNED##,signed,g' ${TMP_INSTALL_SCR}
				fi
			else
				sed -i -e 's,##SIGNED##,_Signed,g' ${TMP_INSTALL_SCR}
			fi
		else
			sed -i -e 's,-##SIGNED##,,g' -e 's,##SIGNED##,,g' ${TMP_INSTALL_SCR}
		fi
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

	# Sign the boot script if not contained in a FIT image
	if [ "${TRUSTFENCE_SIGN_ARTIFACTS}" = "1" ] && [ "${TRUSTFENCE_SIGN_FIT_NXP}" = "0" ]; then
		export CONFIG_SIGN_KEYS_PATH="${TRUSTFENCE_SIGN_KEYS_PATH}"
		[ -n "${TRUSTFENCE_KEY_INDEX}" ] && export CONFIG_KEY_INDEX="${TRUSTFENCE_KEY_INDEX}"
		[ -n "${TRUSTFENCE_SRK_REVOKE_MASK}" ] && export SRK_REVOKE_MASK="${TRUSTFENCE_SRK_REVOKE_MASK}"
		[ -n "${TRUSTFENCE_DEK_PATH}" ] && [ "${TRUSTFENCE_DEK_PATH}" != "0" ] && export CONFIG_DEK_PATH="${TRUSTFENCE_DEK_PATH}"

		# Sign boot script
		TMP_SIGNED_BOOTSCR="$(mktemp ${WORKDIR}/bootscr-signed.XXXXXX)"
		trustfence-sign-artifact.sh -p "${DIGI_SOM}" -b "${DEPLOYDIR}/boot.scr" "${TMP_SIGNED_BOOTSCR}"
		mv "${TMP_SIGNED_BOOTSCR}" "${DEPLOYDIR}/boot.scr"
	fi
}

#
# Clean Yocto generated u-boot symlinks.
#
# Left:
#    u-boot-<defconfig>.<ext>
#    u-boot-<platform>.<ext>-<defconfig>    (needed for imx-boot)
#    u-boot-spl.bin-<platform>-<defconfig>  (needed for imx-boot)
#
uboot_deploy_config:append() {
    rm -f ${DEPLOYDIR}/${UBOOT_SYMLINK} \
        ${DEPLOYDIR}/${UBOOT_BINARY}-${type} \
        ${DEPLOYDIR}/${UBOOT_BINARY}
    ln -sf ${UBOOT_BINARYNAME}-${type}-${PV}-${PR}.${UBOOT_SUFFIX} ${DEPLOYDIR}/${UBOOT_BINARYNAME}-${type}.${UBOOT_SUFFIX}
}
uboot_deploy_spl_config:append() {
    rm -f ${DEPLOYDIR}/${SPL_BINARYFILE}-${type} \
        ${DEPLOYDIR}/${SPL_BINARYFILE} \
        ${DEPLOYDIR}/${SPL_SYMLINK}
}
# Further cleaning for platforms not generating imx-boot
uboot_deploy_config:append:ccimx6ul() {
    rm -f ${DEPLOYDIR}/${UBOOT_SYMLINK}-${type}
}
uboot_deploy_spl_config:append:ccimx6ul() {
    rm -f ${DEPLOYDIR}/${SPL_SYMLINK}-${type}
}

sign_uboot() {
    # This function only applies to CC6, CC6Plus and CC6UL
    [ -z "${UBOOT_CONFIG}" ] && return
    for config in ${UBOOT_MACHINE}; do
        i=$(expr $i + 1)
        for type in ${UBOOT_CONFIG}; do
            j=$(expr $j + 1)
            if [ $j -eq $i ]; then
                cd ${DEPLOYDIR}
                cp -fp ${B}/${config}/SRK_efuses.bin SRK_efuses-${PV}-${PR}.bin
                ln -sf SRK_efuses-${PV}-${PR}.bin SRK_efuses.bin
                cp -fp ${B}/${config}/${UBOOT_BINARYNAME}-dtb-signed.imx ${UBOOT_BINARYNAME}-signed-${type}-${PV}-${PR}.${UBOOT_SUFFIX}
                ln -sf ${UBOOT_BINARYNAME}-signed-${type}-${PV}-${PR}.${UBOOT_SUFFIX} ${UBOOT_BINARYNAME}-signed-${type}.${UBOOT_SUFFIX}
                cp -fp ${B}/${config}/${UBOOT_BINARYNAME}-dtb-usb-signed.imx ${UBOOT_BINARYNAME}-usb-signed-${type}-${PV}-${PR}.${UBOOT_SUFFIX}
                ln -sf ${UBOOT_BINARYNAME}-usb-signed-${type}-${PV}-${PR}.${UBOOT_SUFFIX} ${UBOOT_BINARYNAME}-usb-signed-${type}.${UBOOT_SUFFIX}
                if [ "${TRUSTFENCE_DEK_PATH}" != "0" ]; then
                    cp -fp ${B}/${config}/${UBOOT_BINARYNAME}-dtb-encrypted.imx ${UBOOT_BINARYNAME}-encrypted-${type}-${PV}-${PR}.${UBOOT_SUFFIX}
                    ln -sf ${UBOOT_BINARYNAME}-encrypted-${type}-${PV}-${PR}.${UBOOT_SUFFIX} ${UBOOT_BINARYNAME}-encrypted-${type}.${UBOOT_SUFFIX}
                fi
            fi
        done
        unset j
    done
}

# -----------------------------------------------------------------------------
# Append compile to handle specific device tree compilation
#
do_compile:append:ccmp1() {
	if [ -n "${UBOOT_DEVICETREE}" ]; then
		unset i j
		for config in ${UBOOT_MACHINE}; do
			i=$(expr $i + 1);
			for devicetree in ${UBOOT_DEVICETREE}; do
				# Cleanup previous build artifact
				[ -f "${B}/${config}/dts/dt.dtb" ] && rm "${B}/${config}/dts/dt.dtb"
				# Build target for specific device tree
				oe_runmake -C ${S} O=${B}/${config} DEVICE_TREE=${devicetree} DEVICE_TREE_EXT=${devicetree}.dtb
				# Install specific binary
				for binary in ${UBOOT_BINARIES}; do
					j=$(expr $j + 1);
					if [ $j -eq $i ]; then
						binarysuffix=$(echo ${binary} | cut -d'.' -f2)
						install -m 644 ${B}/${config}/${binary} ${B}/${config}/u-boot-${devicetree}.${binarysuffix}
					fi
				done
				unset j
			done
		done
	fi
}

BOOT_TOOLS = "imx-boot-tools"
BOOT_TOOLS:stm32mpcommon = "u-boot"

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

do_deploy:append:stm32mpcommon() {
	# Deploy u-boot-nodtb.bin and ccmp1x-dvk-xxxx.dtb, to be packaged in fip binary by tf-a
	install -d ${DEPLOYDIR}/${BOOT_TOOLS}
	if [ -n "${UBOOT_DEVICETREE}" ]; then
		for devicetree in ${UBOOT_DEVICETREE}; do
			# Install u-boot dtb
			install -m 644 ${B}/${config}/arch/arm/dts/${devicetree}.dtb ${DEPLOYDIR}/${BOOT_TOOLS}/${FIP_UBOOT_DTB}-${devicetree}.dtb

			if [ "${UBOOT_SIGN_ENABLE}" = "1" ]; then
				# Keep u-boot devicetree without signature
				ubootdevicetree="${B}/${config}/arch/arm/dts/${devicetree}.dtb"
				namewithoutsignature=`echo $ubootdevicetree | sed "s/\.dtb/-without-signature.dtb/g"`
				# Install unsigned U-Boot dtb
				install -m 644 ${namewithoutsignature} ${DEPLOYDIR}/${BOOT_TOOLS}/${FIP_UBOOT_DTB}-${devicetree}-without-signature.dtb
			fi
		done
	fi
	install -m 0777 ${B}/${config}/u-boot-nodtb.bin  ${DEPLOYDIR}/${BOOT_TOOLS}/u-boot-nodtb.bin
}

do_uboot_assemble_fitimage:append:stm32mpcommon() {
	for config in ${UBOOT_MACHINE}; do
		if [ -n "${UBOOT_DEVICETREE}" ] && [ "${UBOOT_SIGN_ENABLE}" = "1" ]; then
			for devicetree in ${UBOOT_DEVICETREE}; do
				# Keep u-boot devicetree without signature
				ubootdevicetree="${B}/${config}/arch/arm/dts/${devicetree}.dtb"
				namewithoutsignature=`echo $ubootdevicetree | sed "s/\.dtb/-without-signature.dtb/g"`
				cp $ubootdevicetree $namewithoutsignature

				# Add image public key in U-Boot dtb file
				fdt_add_pubkey -a "${FIT_HASH_ALG},${FIT_SIGN_ALG}" \
								-k "${UBOOT_SIGN_KEYDIR}" \
								-n "${UBOOT_SIGN_IMG_KEYNAME}" \
								-r "image" \
								"${ubootdevicetree}"

				# Add configuration public key in U-Boot dtb file
				fdt_add_pubkey -a "${FIT_HASH_ALG},${FIT_SIGN_ALG}" \
								-k "${UBOOT_SIGN_KEYDIR}" \
								-n "${UBOOT_SIGN_KEYNAME}" \
								-r "conf" \
								"${ubootdevicetree}"
			done
		fi
	done
}
# Add dependency to make sure that RSA keys generated to sign fitImage are available for u-boot
do_uboot_assemble_fitimage[depends] += " \
	${@'virtual/kernel:do_kernel_generate_rsa_keys' \
	if "stm32mpcommon" in d.getVar('MACHINEOVERRIDES') \
	and "fitImage" in d.getVar('KERNEL_IMAGETYPE') else ''} \
"

FIP_DIR_UBOOT  ?= "/u-boot"

# Deploy u-boot artifacts, to be packaged in fip binary by fip-stm32mp recipe
u_boot_sysroot_populate() {
	local dest="${SYSROOT_DESTDIR}/${FIP_DIR_UBOOT}"

	install -d ${dest}
	for config in ${UBOOT_MACHINE}; do
		if [ -n "${UBOOT_DEVICETREE}" ]; then
			for devicetree in ${UBOOT_DEVICETREE}; do
				# Install u-boot dtb
				install -m 644 ${B}/${config}/arch/arm/dts/${devicetree}.dtb ${dest}/${FIP_UBOOT_DTB}-${devicetree}.dtb
				# Install u-boot binary
				install -m 0777 ${B}/${config}/u-boot-nodtb.bin  ${dest}/u-boot-nodtb-${devicetree}.bin
			done
		fi
	done
}
SYSROOT_PREPROCESS_FUNCS:stm32mpcommon =+ "u_boot_sysroot_populate"
SYSROOT_DIRS:append:stm32mpcommon = " ${FIP_DIR_UBOOT}"

SYSROOT_DIRS += "/boot"

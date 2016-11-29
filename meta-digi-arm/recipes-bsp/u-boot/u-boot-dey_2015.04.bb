# Copyright (C) 2012-2015 Digi International

require u-boot.inc

DESCRIPTION = "Bootloader for Digi platforms"
LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://Licenses/README;md5=c7383a594871c03da76b3707929d2919"

DEPENDS += "dtc-native u-boot-mkimage-native"
DEPENDS += "${@base_conditional('TRUSTFENCE_SIGN', '1', 'trustfence-cst-native', '', d)}"

PROVIDES += "u-boot"

SRCBRANCH = "v2015.04/master"
SRCREV = "${AUTOREV}"

# Select internal or Github U-Boot repo
UBOOT_GIT_URI = "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${DIGI_GIT}u-boot-denx.git', '${DIGI_GITHUB_GIT}/u-boot.git', d)}"

SRC_URI = " \
    ${UBOOT_GIT_URI};branch=${SRCBRANCH} \
"

SRC_URI_append = " \
    file://boot.txt \
    file://install_linux_fw_sd.txt \
"

LOCALVERSION ?= ""
inherit fsl-u-boot-localversion

EXTRA_OEMAKE_append = " KCFLAGS=-fgnu89-inline"

UBOOT_EXTRA_CONF ?= ""

python __anonymous() {
    if (d.getVar("TRUSTFENCE_DEK_PATH", True) not in ["0", None]) and (d.getVar("TRUSTFENCE_SIGN", True) != "1"):
         bb.fatal("Only signed U-Boot images can be encrypted. Generate signed images (TRUSTFENCE_SIGN = \"1\") or remove encryption (TRUSTFENCE_DEK_PATH = \"0\")")
}

do_compile () {
	if [ "${@bb.utils.contains('DISTRO_FEATURES', 'ld-is-gold', 'ld-is-gold', '', d)}" = "ld-is-gold" ] ; then
		sed -i 's/$(CROSS_COMPILE)ld$/$(CROSS_COMPILE)ld.bfd/g' config.mk
	fi

	unset LDFLAGS
	unset CFLAGS
	unset CPPFLAGS

	if [ ! -e ${B}/.scmversion -a ! -e ${S}/.scmversion ]
	then
		echo ${UBOOT_LOCALVERSION} > ${B}/.scmversion
		echo ${UBOOT_LOCALVERSION} > ${S}/.scmversion
	fi

    if [ "x${UBOOT_CONFIG}" != "x" ]
    then
        for config in ${UBOOT_MACHINE}; do
            i=`expr $i + 1`;
            for type in ${UBOOT_CONFIG}; do
                j=`expr $j + 1`;
                if [ $j -eq $i ]
                then
                    oe_runmake O=build_${config} ${config}
                    for var in ${UBOOT_EXTRA_CONF}; do
                        echo "${var}" >> build_${config}/.config
                    done
                    oe_runmake O=build_${config} oldconfig
                    oe_runmake O=build_${config} ${UBOOT_MAKE_TARGET}
                    cp  ${S}/build_${config}/${UBOOT_BINARY}  ${S}/build_${config}/u-boot-${type}.${UBOOT_SUFFIX}

                    # Secure boot artifacts
                    if [ "${TRUSTFENCE_SIGN}" = "1" ]
                    then
                        cp ${S}/build_${config}/u-boot-signed.imx ${S}/build_${config}/u-boot-signed-${type}.${UBOOT_SUFFIX}
                        cp ${S}/build_${config}/u-boot-usb-signed.imx ${S}/build_${config}/u-boot-usb-signed-${type}.${UBOOT_SUFFIX}
			if [ "${TRUSTFENCE_DEK_PATH}" != "0" ]
			then
				cp ${S}/build_${config}/u-boot-encrypted.imx ${S}/build_${config}/u-boot-encrypted-${type}.${UBOOT_SUFFIX}
			fi
                    fi
                fi
            done
            unset  j
        done
        unset  i
    else
        oe_runmake ${UBOOT_MACHINE}
        for var in ${UBOOT_EXTRA_CONF}; do
            echo "${var}" >> .config
        done
        oe_runmake oldconfig
        oe_runmake ${UBOOT_MAKE_TARGET}
    fi

}

TF_BOOTSCRIPT_SEDFILTER = ""
TF_BOOTSCRIPT_SEDFILTER_ccimx6 = "${@tf_bootscript_sedfilter(d)}"
TF_BOOTSCRIPT_SEDFILTER_ccimx6ul = "${@tf_bootscript_sedfilter(d)}"

def tf_bootscript_sedfilter(d):
    tf_initramfs = d.getVar('TRUSTFENCE_INITRAMFS_IMAGE',True) or ""
    return "/^dboot linux/i\setenv boot_initrd true\\nsetenv initrd_file %s-${MACHINE}.cpio.gz.u-boot.tf" % tf_initramfs if tf_initramfs else ""

do_deploy_append() {
	# Remove canonical U-Boot symlinks for ${UBOOT_CONFIG} currently in the form:
	#    u-boot-<platform>.imx-<type>
	#    u-boot-<type>
	# and add a more suitable symlink in the form:
	#    u-boot-<platform>-<config>.imx
	if [ "x${UBOOT_CONFIG}" != "x" ]; then
		for config in ${UBOOT_MACHINE}; do
			i=`expr $i + 1`
			for type in ${UBOOT_CONFIG}; do
				j=`expr $j + 1`
				if [ $j -eq $i ]; then
					cd ${DEPLOYDIR}
					rm -r ${UBOOT_BINARY}-${type} ${UBOOT_SYMLINK}-${type}
					ln -sf u-boot-${type}-${PV}-${PR}.${UBOOT_SUFFIX} u-boot-${type}.${UBOOT_SUFFIX}
					if [ "${TRUSTFENCE_SIGN}" = "1" ]
					then
						install ${S}/build_${config}/SRK_efuses.bin SRK_efuses-${PV}-${PR}.bin
						ln -sf SRK_efuses-${PV}-${PR}.bin SRK_efuses.bin

						install ${S}/build_${config}/u-boot-signed-${type}.${UBOOT_SUFFIX} u-boot-signed-${type}-${PV}-${PR}.${UBOOT_SUFFIX}
						ln -sf u-boot-signed-${type}-${PV}-${PR}.${UBOOT_SUFFIX} u-boot-signed-${type}.${UBOOT_SUFFIX}

						install ${S}/build_${config}/u-boot-usb-signed-${type}.${UBOOT_SUFFIX} u-boot-usb-signed-${type}-${PV}-${PR}.${UBOOT_SUFFIX}
						ln -sf u-boot-usb-signed-${type}-${PV}-${PR}.${UBOOT_SUFFIX} u-boot-usb-signed-${type}.${UBOOT_SUFFIX}

						if [ "${TRUSTFENCE_DEK_PATH}" != "0" ]
						then
							install ${S}/build_${config}/u-boot-encrypted-${type}.${UBOOT_SUFFIX} u-boot-encrypted-${type}-${PV}-${PR}.${UBOOT_SUFFIX}
							ln -sf u-boot-encrypted-${type}-${PV}-${PR}.${UBOOT_SUFFIX} u-boot-encrypted-${type}.${UBOOT_SUFFIX}
						fi
					fi
				fi
			done
			unset  j
		done
		unset  i
	fi

	# DEY firmware install script
	sed -i -e 's,##GRAPHICAL_BACKEND##,${GRAPHICAL_BACKEND},g' ${WORKDIR}/install_linux_fw_sd.txt
	mkimage -T script -n "DEY firmware install script" -C none -d ${WORKDIR}/install_linux_fw_sd.txt ${DEPLOYDIR}/install_linux_fw_sd.scr

	# Boot script for DEY images (reconfigure on-the-fly if TRUSTFENCE is enabled)
	TMP_BOOTSCR="$(mktemp ${WORKDIR}/bootscr.XXXXXX)"
	sed -e "${TF_BOOTSCRIPT_SEDFILTER}" ${WORKDIR}/boot.txt > ${TMP_BOOTSCR}
	mkimage -T script -n bootscript -C none -d ${TMP_BOOTSCR} ${DEPLOYDIR}/boot.scr
	if [ "${TRUSTFENCE_SIGN}" = "1" ]; then
		export CONFIG_SIGN_KEYS_PATH="${TRUSTFENCE_SIGN_KEYS_PATH}"
		[ -n "${TRUSTFENCE_KEY_INDEX}" ] && export CONFIG_KEY_INDEX="${TRUSTFENCE_KEY_INDEX}"
		[ -n "${TRUSTFENCE_DEK_PATH}" ] && [ "${TRUSTFENCE_DEK_PATH}" != "0" ] && export CONFIG_DEK_PATH="${TRUSTFENCE_DEK_PATH}"
		"${STAGING_BINDIR_NATIVE}/trustfence-sign-kernel.sh" -p "${DIGI_FAMILY}" -b "${DEPLOYDIR}/boot.scr" "${DEPLOYDIR}/boot-signed.scr"
		mv ${DEPLOYDIR}/boot-signed.scr ${DEPLOYDIR}/boot.scr
	fi
	rm -f ${TMP_BOOTSCR}
}

COMPATIBLE_MACHINE = "(ccimx6$|ccimx6ul)"

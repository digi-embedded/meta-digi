# Copyright (C) 2017-2023, Digi International Inc.

SUMMARY = "TrustFence signing and encryption scripts"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

SRCBRANCH = "v2020.04/master"
SRCREV = "${AUTOREV}"

S = "${WORKDIR}"

# Select internal or Github U-Boot repo
UBOOT_URI_STASH = "${DIGI_MTK_GIT}/uboot/u-boot-denx.git;protocol=ssh"
UBOOT_URI_GITHUB = "${DIGI_GITHUB_GIT}/u-boot.git;protocol=https"
UBOOT_GIT_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1' , '${UBOOT_URI_STASH}', '${UBOOT_URI_GITHUB}', d)}"

SRC_URI = " \
    ${UBOOT_GIT_URI};branch=${SRCBRANCH} \
    file://trustfence-sign-artifact-nxp.sh \
    file://trustfence-sign-artifact-stm.sh \
    file://sign_hab;name=artifact-hab-sign \
    file://encrypt_hab;name=artifact-hab-encrypt \
    file://sign_ahab;name=artifact-ahab-sign \
    file://encrypt_ahab;name=artifact-ahab-encrypt \
"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install() {
	if [ "${DEY_SOC_VENDOR}" = "NXP" ]; then
		install -d ${D}${bindir}/csf_templates
		if [ "${TRUSTFENCE_SIGN_MODE}" = "AHAB" ]; then
			install -m 0755 sign_ahab ${D}${bindir}/csf_templates/
			install -m 0755 encrypt_ahab ${D}${bindir}/csf_templates/
		elif [ "${TRUSTFENCE_SIGN_MODE}" = "HAB" ]; then
			install -m 0755 sign_hab ${D}${bindir}/csf_templates/
			install -m 0755 encrypt_hab ${D}${bindir}/csf_templates/
		else
			bberror "Unkown TRUSTFENCE_SIGN_MODE value"
			exit 1
		fi

		install -m 0755 git/scripts/csf_templates/* ${D}${bindir}/csf_templates
		install -m 0755 trustfence-sign-artifact-nxp.sh ${D}${bindir}/trustfence-sign-artifact.sh

		# Select U-Boot sign script depending on U-Boot including an SPL image
		if [ -n "${SPL_BINARY}" ]; then
			if [ "${TRUSTFENCE_SIGN_MODE}" = "AHAB" ]; then
				install -m 0755 git/scripts/sign_spl_ahab.sh ${D}${bindir}/trustfence-sign-uboot.sh
			else
				install -m 0755 git/scripts/sign_spl_fit.sh ${D}${bindir}/trustfence-sign-uboot.sh
			fi
		else
			install -m 0755 git/scripts/sign.sh ${D}${bindir}/trustfence-sign-uboot.sh
		fi
	elif [ "${DEY_SOC_VENDOR}" = "STM" ]; then
		install -d ${D}${bindir}
		install -m 0755 trustfence-sign-artifact-stm.sh ${D}${bindir}/trustfence-sign-artifact.sh
	fi
}

FILES:${PN} = "${bindir}"

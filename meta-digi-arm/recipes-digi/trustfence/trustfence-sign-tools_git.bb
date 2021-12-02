# Copyright (C) 2017-2021 Digi International
SUMMARY = "TrustFence signing and encryption scripts"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

DEPENDS = "trustfence-cst coreutils util-linux"
DEPENDS += "${@oe.utils.conditional('TRUSTFENCE_SIGN_MODE', 'AHAB', 'imx-mkimage', '', d)}"

SRCBRANCH = "v2020.04/maint"
SRCREV = "${AUTOREV}"

S = "${WORKDIR}"

# Select internal or Github U-Boot repo
DIGI_LOG_REPO = "u-boot-denx.git"
DIGI_MTK_REPO = "uboot/u-boot-denx.git"
GITHUB_REPO = "u-boot.git"
UBOOT_GIT_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1' , \
					oe.utils.conditional('DIGI_GIT', '${DIGI_LOG_GIT}', '${DIGI_GIT}/${DIGI_LOG_REPO}', '${DIGI_GIT}/${DIGI_MTK_REPO};protocol=ssh', d), \
					'${DIGI_GITHUB_GIT}/${GITHUB_REPO};protocol=https', d)}"

SRC_URI = " \
    ${UBOOT_GIT_URI};branch=${SRCBRANCH} \
    file://trustfence-sign-artifact.sh;name=artifact-sign-script \
    file://sign_hab;name=artifact-hab-sign \
    file://encrypt_hab;name=artifact-hab-encrypt \
    file://sign_ahab;name=artifact-ahab-sign \
    file://encrypt_ahab;name=artifact-ahab-encrypt \
"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install() {
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
	install -m 0755 trustfence-sign-artifact.sh ${D}${bindir}/
	install -m 0755 git/scripts/csf_templates/* ${D}${bindir}/csf_templates

	# Select U-Boot sign script depending on U-Boot including an SPL image
	if [ -n "${SPL_BINARY}" ]; then
		install -m 0755 git/scripts/sign_spl_fit.sh ${D}${bindir}/trustfence-sign-uboot.sh
	else
		install -m 0755 git/scripts/sign.sh ${D}${bindir}/trustfence-sign-uboot.sh
	fi
}

FILES_${PN} = "${bindir}"
BBCLASSEXTEND = "native nativesdk"

# Copyright (C) 2017-2018 Digi International
SUMMARY = "TrustFence signing and encryption scripts"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

DEPENDS = "trustfence-cst coreutils util-linux"

SRCBRANCH = "v2019.04/maint"
SRCREV = "1d066a06cbf2f8cc7fa951a22f6e3ecd3a7666e7"

S = "${WORKDIR}"

# Select internal or Github U-Boot repo
UBOOT_GIT_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1' , '${DIGI_GIT}u-boot-denx.git', '${DIGI_GITHUB_GIT}/u-boot.git;protocol=https', d)}"

SRC_URI = " \
    ${UBOOT_GIT_URI};nobranch=1 \
    file://trustfence-sign-kernel.sh;name=kernel-script \
    file://sign_hab;name=kernel-sign \
    file://encrypt_hab;name=kernel-encrypt \
    file://sign_ahab;name=kernel-sign \
"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install() {
	install -d ${D}${bindir}/csf_templates
	if [ "${TRUSTFENCE_SIGN_MODE}" = "AHAB" ]; then
		install -m 0755 sign_ahab ${D}${bindir}/csf_templates/
	elif [ "${TRUSTFENCE_SIGN_MODE}" = "HAB" ]; then
		install -m 0755 sign_hab ${D}${bindir}/csf_templates/
		install -m 0755 encrypt_hab ${D}${bindir}/csf_templates/
	else
		bberror "Unkown TRUSTFENCE_SIGN_MODE value"
		exit 1
	fi
	install -m 0755 git/scripts/sign.sh ${D}${bindir}/trustfence-sign-uboot.sh
	install -m 0755 trustfence-sign-kernel.sh ${D}${bindir}/
	install -m 0755 git/scripts/csf_templates/* ${D}${bindir}/csf_templates
}

FILES_${PN} = "${bindir}"
BBCLASSEXTEND = "native nativesdk"

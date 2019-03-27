# Copyright (C) 2017-2018 Digi International
SUMMARY = "TrustFence signing and encryption scripts"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

DEPENDS = "trustfence-cst coreutils util-linux"

SRCBRANCH = "v2017.03/maint"
SRCREV = "${AUTOREV}"

S = "${WORKDIR}"

# Select internal or Github U-Boot repo
UBOOT_GIT_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1' , '${DIGI_GIT}u-boot-denx.git', '${DIGI_GITHUB_GIT}/u-boot.git', d)}"

SRC_URI = " \
    ${UBOOT_GIT_URI};branch=${SRCBRANCH} \
    file://trustfence-sign-kernel.sh;name=kernel-script \
    file://sign_uimage;name=kernel-sign \
    file://encrypt_uimage;name=kernel-encrypt \
"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install() {
	install -d ${D}${bindir}/csf_templates
	install -m 0755 trustfence-sign-kernel.sh ${D}${bindir}/
	install -m 0755 sign_uimage ${D}${bindir}/csf_templates/
	install -m 0755 encrypt_uimage ${D}${bindir}/csf_templates/
	install -m 0755 git/scripts/sign.sh ${D}${bindir}/trustfence-sign-uboot.sh
	install -m 0755 git/scripts/csf_templates/sign_uboot ${D}${bindir}/csf_templates
	install -m 0755 git/scripts/csf_templates/encrypt_uboot ${D}${bindir}/csf_templates
}

FILES_${PN} = "${bindir}"
BBCLASSEXTEND = "native nativesdk"

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

DEY_POLICY_PATCHES = " \
    file://0001-Apply-rules-for-DEY-prebuilt-images.patch \
"

SRC_URI += " ${@oe.utils.conditional('DEY_SELINUX_POLICY', '1', '${DEY_POLICY_PATCHES}', '', d)}"

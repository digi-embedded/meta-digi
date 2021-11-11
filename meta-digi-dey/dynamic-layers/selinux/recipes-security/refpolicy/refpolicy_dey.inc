FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

DEY_POLICY_PATCHES = " \
    file://0001-Apply-rules-for-DEY-prebuilt-images.patch \
    file://0002-Make-udevadm_t-executables-run-in-the-udev_t-realm.patch \
"

SRC_URI += " ${@oe.utils.conditional('DEY_SELINUX_POLICY', '1', '${DEY_POLICY_PATCHES}', '', d)}"

#@TYPE: Machine
#@NAME: ConnectCore 6 module.
#@DESCRIPTION: Machine configuration for Digi's ConnectCore 6 module.

include conf/machine/include/imx-digi-base.inc
include conf/machine/include/tune-cortexa9.inc

DIGI_FAMILY = "ccimx6"
SOC_FAMILY  = "mx6:${CPU_FAMILY}:${DIGI_FAMILY}"

# Platform u-boot settings
UBOOT_ENTRYPOINT = "0x10800000"
UBOOT_SUFFIX = "imx"

# wpa_supplicant driver to use in '/etc/network/interfaces'
WPA_DRIVER = "nl80211"

# Firmware
MACHINE_FIRMWARE ?= ""
MACHINE_FIRMWARE_append_mx6q = " firmware-imx-vpu-imx6q"
MACHINE_FIRMWARE_append_mx6dl = " firmware-imx-vpu-imx6d"
MACHINE_FIRMWARE_append = " ${@base_conditional('HAVE_BT', '1' , 'firmware-atheros-ar3k', '', d)}"

MACHINE_EXTRA_RDEPENDS += " \
    e2fsprogs \
    e2fsprogs-mke2fs \
    parted \
    u-boot-dey-fw-utils \
"
MACHINE_EXTRA_RRECOMMENDS += "${MACHINE_FIRMWARE}"
MACHINE_EXTRA_RRECOMMENDS += "fsl-alsa-plugins"

#
# Supported variants
#
# Maintain in sync with the same table in platform local.conf template.
#
# Name        WiFi[w]    BT[b]  Kinetis[k]  Proc[q|dl]      DDR[1024|512|256]
# ---------------------------------------------------------------------------
# wbkq1024       Y         Y        Y       quad/dual          1024MiB       (0x01,0x02)
# wbq512         Y         Y        N       quad/dual           512MiB       (0x03,0x06)
# wbq1024        Y         Y        N       quad/dual          1024MiB       (0x04)
# wq1024         Y         N        N       quad/dual          1024MiB       (0x05)
# wdl256         Y         N        N       duallite/solo       256MiB       (0x07)
# q512           N         N        N       quad/dual           512MiB       (0x08)
# dl256          N         N        N       duallite/solo       256MiB       (0x09)
# wdl512         Y         N        N       duallite/solo       512MiB       (0x0A)
# wbdl1024       Y         Y        N       duallite/solo      1024MiB       (0x0B)

# All variants support
MACHINE_FEATURES += "accel-graphics accel-video"

# Per-variant machine features
MACHINE_FEATURES_append_wbkq1024 = " wifi bluetooth"
MACHINE_FEATURES_append_wbq512 = " wifi bluetooth"
MACHINE_FEATURES_append_wbq1024 = " wifi bluetooth"
MACHINE_FEATURES_append_wq1024 = " wifi"
MACHINE_FEATURES_append_wdl256 = " wifi"
MACHINE_FEATURES_append_q512 = ""
MACHINE_FEATURES_append_dl256 = ""
MACHINE_FEATURES_append_wdl512 = " wifi"
MACHINE_FEATURES_append_wbdl1024 = " wifi bluetooth"

# Per-variant u-boot configuration
UBOOT_MACHINE = ""
UBOOT_MACHINE_wbkq1024 = "ccimx6qsbc_config"
UBOOT_MACHINE_wbq512 = "ccimx6qsbc512MB_config"
UBOOT_MACHINE_wbq1024 = "ccimx6qsbc_config"
UBOOT_MACHINE_wq1024 = "ccimx6qsbc_config"
UBOOT_MACHINE_wdl256 = "ccimx6dlsbc256MB_config"
UBOOT_MACHINE_q512 = "ccimx6qsbc512MB_config"
UBOOT_MACHINE_dl256 = "ccimx6dlsbc256MB_config"
UBOOT_MACHINE_wdl512 = "ccimx6dlsbc512MB_config"
UBOOT_MACHINE_wbdl1024 = "ccimx6dlsbc_config"

# Per-variant cpu family
CPU_FAMILY = ""
CPU_FAMILY_wbkq1024 = "mx6q"
CPU_FAMILY_wbq512 = "mx6q"
CPU_FAMILY_wbq1024 = "mx6q"
CPU_FAMILY_wq1024 = "mx6q"
CPU_FAMILY_wdl256 = "mx6dl"
CPU_FAMILY_q512 = "mx6q"
CPU_FAMILY_dl256 = "mx6dl"
CPU_FAMILY_wdl512 = "mx6dl"
CPU_FAMILY_wbdl1024 = "mx6dl"

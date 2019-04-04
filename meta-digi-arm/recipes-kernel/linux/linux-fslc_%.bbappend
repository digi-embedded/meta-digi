FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI_append_ccimx6ul_use-mainline-bsp = " \
    file://0001-ARM-Add-support-for-the-ConnectCore-6UL-System-On-Mo.patch \
    file://0002-mach-imx-pm-imx6-Add-hooks-for-board-specific-implem.patch \
    file://0003-imx6ul-Add-MCA-core-I2C-driver-support.patch \
    file://0004-imx6ul-Add-MCA-GPIO-support-for-the-ConnectCore-6UL-.patch \
    file://0005-imx6ul-Add-MCA-IOMUX-support-to-the-ConnectCore-6UL-.patch \
    file://0006-imx6ul-Add-MCA-watchdog-support-for-the-ConnectCore-.patch \
    file://0007-imx6ul-Add-MCA-ADC-support-for-ConnectCore-6UL-SOM-a.patch \
    file://0008-imx6ul-Add-MCA-tamper-support-for-ConnectCore-6UL-SO.patch \
    file://0009-imx6ul-Add-MCA-UART-support-for-ConnectCore-6UL-SOM-.patch \
    file://0010-imx6ul-Add-RTC-MCA-support-for-ConnectCore-6UL-SOM.patch \
    file://0011-imx6ul-Add-MCA-power-key-support-for-ConnectCore-6UL.patch \
"

# Copyright (C) 2012 Digi International

UBOOT_ENTRYPOINT  = "0x40008000"

require recipes-kernel/linux/linux-dtb.inc

include linux-dey.inc
include linux-dey-rev_${PV}.inc

PR = "${DISTRO}.${INC_PR}.0"

LOCALVERSION_mxs = "mxs"
LOCALVERSION_cpx2_mxs = "mxs+gateways"

# Kernel configuration fragments
KERNEL_CFG_FRAGS ?= ""
KERNEL_CFG_FRAGS_append = " ${@base_conditional('HAVE_EXAMPLE', '1' , 'file://config-spidev.cfg', '', d)}"

SRC_URI += " \
    file://defconfig \
    ${KERNEL_CFG_FRAGS} \
"

S = "${WORKDIR}/git"

KERNEL_DEVICETREE = "${S}/arch/arm/boot/dts/${DTSNAME}.dts"
KERNEL_EXTRA_ARGS = "LOADADDR=${UBOOT_LOADADDRESS}"

config_dts() {
	if [ "${1}" = "enable" ]; then
		sed  -i -e "/${2}/{s,^///include,/include,g}" ${KERNEL_DEVICETREE}
	elif [ "${1}" = "disable" ]; then
		sed  -i -e "/${2}/{s,^/include,///include,g}" ${KERNEL_DEVICETREE}
	fi
}

do_update_dts() {
	if [ -n "${HAVE_WIFI}" ]; then
		config_dts enable  '_ssp2_mmc_wifi.dtsi'
	else
		config_dts disable '_ssp2_mmc_wifi.dtsi'
	fi
	if [ -n "${HAVE_EXT_ETH}" ]; then
		config_dts enable  '_ethernet1.dtsi'
	else
		config_dts disable '_ethernet1.dtsi'
	fi
	if [ -n "${HAVE_BT}" ]; then
		config_dts enable  '_auart0_bluetooth.dtsi'
	else
		config_dts disable '_auart0_bluetooth.dtsi'
	fi
	if [ -n "${HAVE_1WIRE}" ]; then
		config_dts enable  '_onewire_i2c1.dtsi'
		config_dts disable '_auart2_4wires.dtsi'
	else
		config_dts disable '_onewire_i2c1.dtsi'
	fi
	if [ -n "${HAVE_GUI}" ]; then
		# Enable LCD
		config_dts enable  '_display_'
		config_dts disable '_auart1_'
		# Enable touch
		config_dts enable  '_lradc_touchscreen'
		config_dts disable '_ssp1_'
		config_dts disable '_auart1_4wires'
		config_dts disable '_ethernet0_leds'
	fi
	if [ -n "${HAVE_EXAMPLE}" ]; then
		config_dts enable  'ssp1_spi_gpio.dtsi'
		config_dts enable  'ssp1_spi_gpio_spidev.dtsi'
	fi
}
addtask update_dts before do_install after do_sizecheck

FILES_kernel-image += "/boot/config*"

COMPATIBLE_MACHINE = "(mxs)"

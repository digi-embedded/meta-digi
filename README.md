# Digi Embedded Yocto (DEY) 4.0
## Release 4.0-r6

This document provides information about Digi Embedded Yocto,
Digi International's professional embedded Yocto development environment.

Digi Embedded Yocto 4.0 is based on the Yocto Project(TM) 4.0 (Kirkstone) release.

For a full list of supported features and interfaces please refer to the
online documentation.

# Tested OS versions

The current release has been verified and tested with the following
OS versions:

* Ubuntu 18.04
* Ubuntu 22.04

# Supported Platforms

Software for the following hardware platforms is in production support:

## ConnectCore 93
* ConnectCore 93 System-on-Module (SOM)
  * [CC-WMX-YC7D-KN](https://www.digi.com/products/models/cc-wmx-yc7d-kn)
* ConnectCore 93 Development Kit (DVK)
  * [CC-WMX93-KIT](https://www.digi.com/products/models/cc-wmx93-kit) ([Get Started](https://www.digi.com/resources/documentation/digidocs/embedded/dey/4.0/cc93/yocto-gs_index))

## ConnectCore MP13
* ConnectCore MP13 System-on-Module (SOM)
  * [CC-WST-DX58-NK](https://www.digi.com/products/models/cc-wst-dx58-nk)
  * [CC-ST-DX58-ZK](https://www.digi.com/products/models/cc-st-dx58-zk)
* ConnectCore MP13 Development Kit (DVK)
  * [CC-WMP133-KIT](https://www.digi.com/products/models/cc-wmp133-kit) ([Get Started](https://www.digi.com/resources/documentation/digidocs/embedded/dey/4.0/ccmp13/yocto-gs_index))

## ConnectCore MP15
* ConnectCore MP15 System-on-Module (SOM)
  * [CC-WST-DW69-NM](https://www.digi.com/products/models/cc-wst-dw69-nm)
  * [CC-ST-DW69-ZM](https://www.digi.com/products/models/cc-st-dw69-zm)
* ConnectCore MP15 Development Kit (DVK)
  * [CC-WMP157-KIT](https://www.digi.com/products/models/cc-wmp157-kit) ([Get Started](https://www.digi.com/resources/documentation/digidocs/embedded/dey/4.0/ccmp15/yocto-gs_index))

## ConnectCore 8M Mini
* ConnectCore 8M Mini System-on-Module (SOM)
  * [CC-WMX-ET8D-NN](https://www.digi.com/products/models/cc-wmx-et8d-nn)
  * [CC-WMX-ET7D-NN](https://www.digi.com/products/models/cc-wmx-et7d-nn)
  * [CC-MX-ET8D-ZN](https://www.digi.com/products/models/cc-mx-et8d-zn)
  * [CC-MX-ET7D-ZN](https://www.digi.com/products/models/cc-mx-et7d-zn)
* ConnectCore 8M Mini Development Kit (DVK)
  * [CC-WMX8MM-KIT](https://www.digi.com/products/models/cc-wmx8mm-kit) ([Get Started](https://www.digi.com/resources/documentation/digidocs/embedded/dey/4.0/cc8mmini/yocto-gs_index))

## ConnectCore 8M Nano
* ConnectCore 8M Nano System-on-Module (SOM)
  * [CC-WMX-FS7D-NN](https://www.digi.com/products/models/cc-wmx-fs7d-nn)
  * [CC-WMX-FR6D-NN](https://www.digi.com/products/models/cc-wmx-fr6d-nn)
  * [CC-MX-FS7D-ZN](https://www.digi.com/products/models/cc-mx-fs7d-zn)
  * [CC-MX-FR6D-ZN](https://www.digi.com/products/models/cc-mx-fr6d-zn)
* ConnectCore 8M Nano Development Kit (DVK)
  * [CC-WMX8MN-KIT](https://www.digi.com/products/models/cc-wmx8mn-kit) ([Get Started](https://www.digi.com/resources/documentation/digidocs/embedded/dey/4.0/cc8mnano/yocto-gs_index))

## ConnectCore 8X
* ConnectCore 8X System-on-Module (SOM)
  * [CC-WMX-JM8E-NN](https://www.digi.com/products/models/cc-wmx-jm8e-nn)
  * [CC-MX-JM8D-ZN](https://www.digi.com/products/models/cc-mx-jm8d-zn)
  * [CC-MX-JM7D-ZN](https://www.digi.com/cc8x)
  * [CC-WMX-JM7D-NN](https://www.digi.com/products/models/cc-wmx-jm7d-nn)
  * [CC-MX-JQ6D-ZN](https://www.digi.com/cc8x)
  * [CC-MX-JQ7D-ZN](https://www.digi.com/cc8x)
  * [CC-WMX-JQ7D-ZN](https://www.digi.com/cc8x)
* ConnectCore 8X SBC Pro
  * [CC-WMX8-PRO](https://www.digi.com/products/embedded-systems/single-board-computers/digi-connectcore-8x-sbc-pro) ([Get Started](https://www.digi.com/resources/documentation/digidocs/embedded/dey/4.0/cc8x/yocto-gs_index))

## ConnectCore 6UL
* ConnectCore 6UL System-on-Module (SOM)
  * [CC-WMX-JN7A-NE](https://www.digi.com/products/models/cc-wmx-jn7a-ne)
  * [CC-WMX-JN69-NN](https://www.digi.com/products/models/cc-wmx-jn69-nn)
  * [CC-WMX-JN59-NN](https://www.digi.com/products/models/cc-wmx-jn59-nn)
  * [CC-WMX-JN58-NE](https://www.digi.com/products/models/cc-wmx-jn58-ne)
  * [CC-MX-JN7A-Z1](https://www.digi.com/products/models/cc-mx-jn7a-z1)
  * [CC-MX-JN69-ZN](https://www.digi.com/products/models/cc-mx-jn69-zn)
  * [CC-MX-JN58-Z1](https://www.digi.com/products/models/cc-mx-jn58-z1)

* ConnectCore 6UL SBC Pro
  * [CC-WMX6UL-KIT](https://www.digi.com/products/models/cc-wmx6ul-kit) ([Get Started](https://www.digi.com/resources/documentation/digidocs/embedded/dey/4.0/cc6ul/yocto-gs_index))
  * [CC-SBP-WMX-JN58](https://www.digi.com/products/models/cc-sbp-wmx-jn58)
  * [CC-SBP-WMX-JN7A](https://www.digi.com/products/models/cc-sbp-wmx-jn7a)

## ConnectCore 6 Plus
* ConnectCore 6 Plus System-on-Module (SOM)
  * [CC-WMX-KK8D-TN](https://www.digi.com/products/models/cc-wmx-kk8d-tn)
* ConnectCore 6 Plus professional development kit
  * [CC-WMX6P-KIT](https://www.digi.com/products/models/cc-wmx6p-kit) ([Get Started](https://www.digi.com/resources/documentation/digidocs/embedded/dey/4.0/cc6plus/yocto-gs_index))

## ConnectCore 6
* ConnectCore 6 System-on-Module (SOM)
  * [CC-WMX-J97C-TN](https://www.digi.com/products/models/cc-wmx-j97c-tn)
  * [CC-WMX-L96C-TE](https://www.digi.com/products/models/cc-wmx-l96c-te)
  * [CC-WMX-L87C-TE](https://www.digi.com/products/models/cc-wmx-l87c-te)
  * [CC-MX-L76C-Z1](https://www.digi.com/products/models/cc-mx-l76c-z1)
  * [CC-MX-L86C-Z1](https://www.digi.com/products/models/cc-mx-l86c-z1)
  * [CC-MX-L96C-Z1](https://www.digi.com/products/models/cc-mx-l96c-z1)
  * [CC-WMX-L76C-TE](https://www.digi.com/products/models/cc-wmx-l76c-te)
  * CC-WMX-K87C-FJA
  * CC-WMX-K77C-TE
  * CC-WMX-L97D-TN
  * CC-WMX-J98C-FJA
  * CC-WMX-J98C-FJA-1
* ConnectCore 6 Jumpstart Development Kit (SBC with Connectore 6 module)
  * [CC-WMX6-KIT](https://www.digi.com/products/models/cc-wmx6-kit) ([Get Started](https://www.digi.com/resources/documentation/digidocs/embedded/dey/4.0/cc6/yocto-gs_index))
  * [CC-SB-WMX-J97C-1](https://www.digi.com/products/models/cc-sb-wmx-j97c-1)
  * [CC-SB-WMX-L87C-1](https://www.digi.com/products/models/cc-sb-wmx-l87c-1)
  * [CC-SB-WMX-L76C-1](https://www.digi.com/products/models/cc-sb-wmx-l76c-1)

# Installation

Digi Embedded Yocto is composed of a set of different Yocto layers that work in
parallel. The layers are specified on a [manifest](https://github.com/digi-embedded/dey-manifest/blob/kirkstone/default.xml) file.

To install, please follow the instructions at the dey-manifest [README](https://github.com/digi-embedded/dey-manifest)

# Documentation

Documentation is available online at https://www.digi.com/resources/documentation/digidocs/embedded/

# Downloads

* Demo images: https://ftp1.digi.com/support/digiembeddedyocto/4.0/r5/images/
* Software Development Kit (SDK): https://ftp1.digi.com/support/digiembeddedyocto/4.0/r5/sdk/

# Release Changelog

## 4.0-r5

* ST-based platforms
  * Add support to boot signed FIT images.
  * Add support to EGLFS backend for CCMP15 platform
  * Add overlay to enable Cortex-M coprocessor
* NXP-based platforms
  * Updated BSP for ConnectCore 93
    * U-Boot v2023.04 (based on tag 'lf-6.1.55-2.2.0' by NXP)
    * Linux kernel v6.1.55 (based on tag 'lf-6.1.55-2.2.0' by NXP)
  * Add overlay to enable Cortex-M coprocessor
  * Added preliminary TrustFence support for ConnectCore 93
* Add support to LVGL based images
* Improved ConnectCore Cloud Services (CCCS):
  * Data backlog support to locally store samples when it is not possible to upload them
  * CCCS API to set the device maintenance state of devices
  * CCCS API to upload binary data points
  * Report to Remote Manager when a device is using a Wi-Fi connection
  * Improve firmware download speed
  * Configuration file:
    * Use default values if configuration file is not provided
    * Allow to disable firmware update service
* Improved SWU package generation and support:
  * Generalized and simplified recipes to generate the SWU packages using a custom class
  * Added support to update bootloader using software update (SWU)
* Update Python XBee library
* Bootcount feature is now disabled by default.
* General bug fixing and improvements

## 4.0-r4

* ST-based platforms
  * Reworked NAND partition table and disabled UBI Fastmap mechanism.
  * Updated BSP
    * Updated Trusted Firmware ARM (based on tag 'v2.6-stm32mp-r2.1' by ST)
    * Updated OP-TEE (based on tag 'v2.6-stm32mp-r2.1' by ST)
    * Updated U-Boot v2021.10 (based on tag 'v2.6-stm32mp-r2.1' by ST)
    * Updated Linux kernel v5.15.118 (based on tag 'v2.6-stm32mp-r2.1' by ST)
    * Updated Bluetooth firmware to comply with FCC and CE regulations (release 001.001.025 build 0155 from Murata)
    * Restricted Wi-Fi regulatory domain to US only
  * Re-enable auto-mount of microSD card on kernel boot
  * Add sdcard generation support
* NXP-based platforms
  * Added support to ConnectCore 6/6 Plus
    * U-Boot v2017.03
    * Linux kernel v5.15.71 (based on tag 'lf-5.15.71-2.2.0' by NXP)
  * Added support to ConnectCore 8X
    * U-Boot v2020.04
    * Migrate imx-boot format to use SPL support to use the same binary for all memory variants
    * Linux kernel v5.15.71 (based on tag 'lf-5.15.71-2.2.0' by NXP)
  * Updated support to ConnectCore 93
    * U-Boot v2023.04 (based on tag 'lf-6.1.22-2.0.0' by NXP)
    * Linux kernel v6.1.22 (based on tag 'lf-6.1.22-2.0.0' by NXP)
    * Arm Ethos-U65 Neural Processing Unit (NPU) acceleration for machine learning
  * Updated QT6 to v6.5
* Improved bootcount support:
  * Bootcount feature is now always active and not only after a dual boot firmware update. This new configuration applies to all platforms except for CC6 based devices, which will keep the previous behavior.
  * Bootcount value is now stored in registers with soft reset protection to maintain the value:
    * CC6 devices: bootcount is still stored in the U-Boot environment.
    * CC6UL/CC8X/CC8M devices: bootcount is stored in the MCA NVMEM registers.
    * CCMP1/CC93 devices: bootcount is stored in the DVK RTC NVMEM registers.
  * Added a bootcount command to U-Boot and Linux to manage the boocount value.
  * Moved 'altboot' script functionality to 'altbootcmd' in U-Boot and removed all the 'altboot' scripts
* Improved SWU package support and generation:
  * Generalized and simplified recipes to generate the SWU packages using a custom class
  * Added support to create a new SWU package based on files to update only specific parts of the active system
  * Added support to create a new SWU package based on binary differences to update read-only squashfs rootfs partitions
* New ConnectCore Cloud Services (CCCS) application design:
  * Daemon ('cccsd') with general services (cloud connection, files system, system monitor, firmware update, remote command line) and capable of communicate with other applications via CCCS API to send and receive data
  * CCCS API applications communicating with CCCS daemon to send data points to the cloud and receive data requests from the cloud
    Default images include:
    * ConnectCore Cloud Services get started demo ('cccs-gs-demo')
    * Example applications in 'dey-examples': 'cccs-upload-data-points-example' and 'cccs-data-request-example'
* General bug fixing and improvements

## 4.0-r3

* ST-based platforms
  * Added initial TrustFence support
  * Fixed Ethernet PHY pinctrl resuming from deep sleep
  * Adjust CAN bus parent clock to achieve more accurate baudrates
  * Add DT overlay for Bluetooth raw test mode
  * Adjust NAND lines speed settings
  * Add specific kernel driver for Marvell Ethernet PHY on DVK
  * Fix race condition on bringup of LAN87xx Ethernet PHY
  * Disable auto-mount of microSD card to avoid race condition on kernel boot
* NXP-based platforms
  * Added support to ConnectCore 93
    * U-Boot v2022.04 (based on tag 'lf-5.15.71-2.2.0' by NXP)
    * Linux kernel v5.15.71 (based on tag 'lf-5.15.71-2.2.0' by NXP)
    * QT6 6.3.2
  * Fix PMIC regulators suspend state on ConnectCore 8M Nano
  * Fix clock initialization issue on LAN8710/20 PHY on ConnectCore 6UL
* General bug fixing and improvements

## 4.0-r2

* Added webkit support
* ST-based platforms
  * Added support to ConnectCore MP13
  * Updated BSP
    * Updated Trusted Firmware ARM
    * Updated OP-TEE
    * Updated U-Boot v2021.10
    * Updated Linux kernel v5.15.67 (based on tag 'v5.15-stm32mp-r2' by ST)
    * Updated Wifi driver (based on 'v5.15.58-2023_0222' release from Cypress)
    * Updated Wifi firmware to 'imx-kirkstone-fafnir_r1.0' release from Murata
* NXP-based platforms
  * Added support to ConnectCore 8M Mini
  * Added support to ConnectCore 8M Nano
  * Updated BSP
    * Updated U-Boot v2020.04
    * Updated Linux kernel v5.15.71 (based on tag 'lf-5.15.71-2.2.0' by NXP)
  * Updated MCA firmware for ConnectCore 6UL to v1.22.
  * Added Qt 6.3.2 support for ConnectCore 8M platforms

## 4.0-r1

* Release based on [Yocto 4.0 (Kirkstone)](https://www.yoctoproject.org/software-overview/downloads) including:
  * New toolchain based on GLIBC-2.35
  * Updated bluez5 to v5.65
  * Updated busybox to v1.32.0
  * Updated NetworkManager to v1.36.2
  * Updated gstreamer1.0 to v1.20.3
  * Updated busybox to v1.35.0
  * Updated OpenSSL to v3.0.7
  * Package upgrades and security fixes
* Added support for ConnectCore MP15 platform
* Updated kernel version to v5.15.52 for i.MX6UL platforms


# Known Issues and Limitations

This is a list of known issues and limitations at the time of release. An
updated list can be found on the online documentation.

* Firmware update
  * The software update package must be located in the root level of the
    update media (subfolders are not yet supported).
* Cloud Connector
  * Remote file system management fails with long file names and paths
    (over 255 characters).
* Wireless
  * Performance of the wireless interface is reduced when using concurrent mode,
    as the wireless interface is shared between several different
    functionalities.
  * When using wireless concurrent mode, Digi recommends you keep the different
    modes on the same frequency channels. For example, when configuring access
    point mode on channel 36 in the 5GHz band, connect to the same channel both
    in station mode and Wi-Fi direct so that the radio performance is optimized.
  * When working as an access point, DFS-capable channels in the 5GHz band are
    not supported.
  * For P2P connections Digi recommends "Negotiated GO" modes. The QCA6564
    devices (ConnectCore 6UL, ConnectCore 6 Plus, and ConnectCore 8M Nano) fail
    to join autonomous groups.

## ConnectCore MP15/MP13

* ConnectCore MP1 System-on-Module (SOM)
  * Wireless
    * P2P on the ConnectCore MP1 doesn't currently work in concurrency with
      other modes (station or SoftAP).

## ConnectCore 6UL

* ConnectCore 6UL System-on-Module (SOM)
  * The UART connected to the Bluetooth chip on early versions of the ConnectCore
    6UL system-on-module (hardware version < 4) cannot properly execute flow
    control. To work around this issue, UART1 of these SOM versions has been
    configured to operate at 115200 bps and without hardware flow control,
    reducing the maximum throughput of this interface.
  * The QCA6564 wireless chip does not support Wake On Wireless LAN.

# Support Contact Information

For support questions please contact Digi Technical Support:

* [Enterprise Support](https://mydigi.secure.force.com/customers/)
* [Product Technical Support](https://www.digi.com/support#support-tools)
* [Support forum](https://www.digi.com/support/forum/)

When you contact Digi Technical Support, include important system details and
device information to help Digi resolve the issue more quickly.

1. In the device, run the command 'sysinfo'. This generates the following file:
   /tmp/&lt;current timestamp>.txt.gz.
2. Attach the &lt;current timestamp>.txt.gz file to your support ticket.

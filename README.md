# Digi Embedded Yocto (DEY) 5.0
## Release 5.0-r2

This document provides information about Digi Embedded Yocto,
Digi International's professional embedded Yocto development environment.

Digi Embedded Yocto 5.0 is based on the Yocto Project(TM) 5.0 (Scarthgap) release.

For a full list of supported features and interfaces please refer to the
online documentation.

# Tested OS versions

The current release has been verified and tested with the following
OS versions:

* Ubuntu 20.04
* Ubuntu 22.04

# Supported Platforms

Software for the following hardware platforms is in production support:

## ConnectCore MP25
* ConnectCore MP25 System-on-Module (SOM)
  * [CC-WST-J17D-NK](https://www.digi.com/products/models/cc-wst-j17d-nk)
  * [CC-ST-J17D-ZK](https://www.digi.com/products/models/cc-st-j17d-zk)
* ConnectCore MP25 Development Kit (DVK)
  * [CC-WMP255-KIT](https://www.digi.com/products/models/cc-wmp255-kit) ([Get Started](https://www.digi.com/resources/documentation/digidocs/embedded/dey/5.0/ccmp25/yocto-gs_index))

## ConnectCore 91
* ConnectCore 91 System-on-Module (SOM)
  * [CC-WMX-F26D-L1](https://www.digi.com/products/models/cc-wmx-f26d-l1)
  * [CC-MX-F26D-Z1](https://www.digi.com/products/models/cc-mx-f26d-z1)

## ConnectCore 93
* ConnectCore 93 System-on-Module (SOM)
  * [CC-WMX-YC7D-KN](https://www.digi.com/products/models/cc-wmx-yc7d-kn)
  * [CC-MX-YC7D-ZN](https://www.digi.com/products/models/cc-mx-yc7d-zn)
  * [CC-WMX-ZC6D-L1](https://www.digi.com/products/models/cc-wmx-zc6d-l1)
  * [CC-MX-ZC6D-Z1](https://www.digi.com/products/models/cc-mx-zc6d-z1)
* ConnectCore 93 Development Kit (DVK)
  * [CC-WMX93-KIT](https://www.digi.com/products/models/cc-wmx93-kit) ([Get Started](https://www.digi.com/resources/documentation/digidocs/embedded/dey/5.0/cc93/yocto-gs_index))

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
  * [CC-WMX8MM-KIT](https://www.digi.com/products/models/cc-wmx8mm-kit) ([Get Started](https://www.digi.com/resources/documentation/digidocs/embedded/dey/5.0/cc8mmini/yocto-gs_index))

## ConnectCore 8M Nano
* ConnectCore 8M Nano System-on-Module (SOM)
  * [CC-WMX-FS7D-NN](https://www.digi.com/products/models/cc-wmx-fs7d-nn)
  * [CC-WMX-FR6D-NN](https://www.digi.com/products/models/cc-wmx-fr6d-nn)
  * [CC-MX-FS7D-ZN](https://www.digi.com/products/models/cc-mx-fs7d-zn)
  * [CC-MX-FR6D-ZN](https://www.digi.com/products/models/cc-mx-fr6d-zn)
* ConnectCore 8M Nano Development Kit (DVK)
  * [CC-WMX8MN-KIT](https://www.digi.com/products/models/cc-wmx8mn-kit) ([Get Started](https://www.digi.com/resources/documentation/digidocs/embedded/dey/5.0/cc8mnano/yocto-gs_index))

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
  * [CC-WMX8-PRO](https://www.digi.com/products/embedded-systems/single-board-computers/digi-connectcore-8x-sbc-pro) ([Get Started](https://www.digi.com/resources/documentation/digidocs/embedded/dey/5.0/cc8x/yocto-gs_index))

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
  * [CC-WMX6UL-KIT](https://www.digi.com/products/models/cc-wmx6ul-kit) ([Get Started](https://www.digi.com/resources/documentation/digidocs/embedded/dey/5.0/cc6ul/yocto-gs_index))
  * [CC-SBP-WMX-JN58](https://www.digi.com/products/models/cc-sbp-wmx-jn58)
  * [CC-SBP-WMX-JN7A](https://www.digi.com/products/models/cc-sbp-wmx-jn7a)

# Installation

Digi Embedded Yocto is composed of a set of different Yocto layers that work in
parallel. The layers are specified on a [manifest](https://github.com/digi-embedded/dey-manifest/blob/scarthgap/default.xml) file.

To install, please follow the instructions at the dey-manifest [README](https://github.com/digi-embedded/dey-manifest)

# Documentation

Documentation is available online at https://www.digi.com/resources/documentation/digidocs/embedded/

# Downloads

* Demo images: https://ftp1.digi.com/support/digiembeddedyocto/5.0/r2/images/
* Software Development Kit (SDK): https://ftp1.digi.com/support/digiembeddedyocto/5.0/r2/sdk/

# Release Changelog

## 5.0-r2

* ST-based platforms
  * Added support to ConnectCore MP13
  * Added support to ConnectCore MP15
  * Updated BSP
    * Trusted Firmware ARM v2.8 (based on tag 'v2.10-stm32mp-r1.2' by ST)
    * OP-TEE v4.0.0 (based on tag '4.0.0-stm32mp-r1.2' by ST)
    * U-Boot v2023.10 (based on tag 'v2023.10-stm32mp-r1.2' by ST)
    * Updated X-LINUX-AI software package (based on tag 'v6.0.1' by ST)
    * Updated Wifi driver (based on 'v6.1.110-2025_0602' release from Cypress)
    * Updated Wifi firmware to 'imx-scarthgap-jaculus_r1.1' release from Murata
      * 2FY Wireless chip: v28.10.387.16
      * 2AE Wireless chip: v13.10.246.356
  * Added initial TrustFence support for ConnectCore MP2
  * Added Qt 6.8.4 support for ConnectCore MP1 platforms
  * Added support to new countries on ConnectCore MP1 World CLM blob file
  * Added real-time support
* NXP-based platforms
  * Added support to ConnectCore 8M Mini
  * Added support to ConnectCore 8M Nano
  * Added real-time support
  * Updated i.MX GStreamer stack to 1.24.7.imx
  * Applied certified power limits on ConnectCore 93 and 91
* TrustFence
   * Added support to install signed/encrypted images from uuu install script
* Added support to Flutter framework
  * Added new image recipe for Flutter graphical applications
* Removed Crank framework support
* Added UBI health monitor service (for NAND-based SOMs)
* BTRFS filesystem support for LXC incremental snapshots
* Created installer ZIP by default, and not *.sdcard image
* Enabled SSL/TLS support on vsftpd daemon by default
* General bug fixing and improvements

## 5.0-r1

* Release based on [Yocto 5.0 (Scarthgap)](https://www.yoctoproject.org/software-overview/downloads) including:
  * New toolchain based on GLIBC-2.39
  * Updated bluez5 to v5.72
  * Updated busybox to v1.36.1
  * Updated NetworkManager to v1.46.0
  * Updated gstreamer1.0 to v1.22.12
  * Updated OpenSSL to v3.2.3
  * Package upgrades and security fixes
* ST-based platforms
  * Added support to ConnectCore MP25
  * Updated BSP
    * Updated Trusted Firmware ARM v2.10 (based on tag 'v2.10-stm32mp-r1' by ST)
    * Updated OP-TEE v4.0.0 (based on tag '4.0.0-stm32mp-r1' by ST)
    * Updated U-Boot v2023.10 (based on tag 'v2023.10-stm32mp-r1' by ST)
    * Updated Linux kernel v6.6.48 (based on tag 'v6.6-stm32mp-r1.1' by ST)
    * Updated Wifi driver (based on 'v6.1.97-2024_1115' release from Cypress)
    * Updated Wifi firmware to 'imx-scarthgap-jaculus_r1.0' release from Murata
* NXP-based platforms
  * Added support to ConnectCore 6UL
  * Added support to ConnectCore 8X
  * Added support to ConnectCore 91
  * Added support to ConnectCore 93
  * Updated BSP
    * Updated U-Boot v2024.04 (based on tag 'lf-6.6.52-2.2.0' by NXP)
    * Updated Linux kernel v6.6.52 (based on tag 'lf-6.6.52-2.2.0' by NXP)


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
    devices fail to join autonomous groups.
* Mouse input does not work in Flutter-based applications. When launching
  Flutter applications, the system fails to register mouse input. This
  prevents interaction with graphical elements and UI components, impacting
  use cases that rely on pointer input.
  Using a panel with a touchscreen prevents the issue from appearing.
  This issue has been reported to the community, and we are waiting for a fix.
* Mouse clicks stop working in WebKit after running the Aquarium demo or
  video settings icon. After executing the Aquarium WebGL demo in WebKit,
  mouse click events are no longer recognized in subsequent browsing sessions.
  An application restart is required to recover proper mouse functionality.
  This issue has been reported to the community, and we are working together
  on a fix.

## ConnectCore 93

* ConnectCore 93 System-on-Module (SOM)
  * Trustfence
    * It is not possible to close a device using U-Boot v2024.04 for Secure boot
      on a device with CPU revision A0. However, it is possible to boot v2024.04
      signed images on an already closed device.

## ConnectCore 6UL

* ConnectCore 6UL System-on-Module (SOM)
  * The UART connected to the Bluetooth chip on early versions of the ConnectCore
    6UL system-on-module (hardware version < 4) cannot properly execute flow
    control. To work around this issue, UART1 of these SOM versions has been
    configured to operate at 115200 bps and without hardware flow control,
    reducing the maximum throughput of this interface.
  * The QCA6564 wireless chip does not support Wake On Wireless LAN.

## ConnectCore MP25

* MMC0 input/output errors may appear during extended suspend/resume cycles.
* The system may fail to power off correctly, eventually falling back to a watchdog
  reset.

## ConnectCore MP13

* The power button becomes unresponsive after one power-off/power-on cycle.
  A system reboot is required to restore normal functionality.
  The issue originates from the OP-TEE/ATF firmware and will be addressed in
  the next DEY release.
  If you need further assistance, please contact Digi Technical Support.

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

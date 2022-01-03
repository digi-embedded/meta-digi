# Digi Embedded Yocto (DEY) 3.2
## Release 3.2-r2

This document provides information about Digi Embedded Yocto,
Digi International's professional embedded Yocto development environment.

Digi Embedded Yocto 3.2 is based on the Yocto Project(TM) 3.2 (Gatesgarth) release.

For a full list of supported features and interfaces please refer to the
online documentation.

# Tested OS versions

The current release has been verified and tested with the following
OS versions:

* Ubuntu 18.04

# Supported Platforms

Software for the following hardware platforms is in production support:

## ConnectCore 8M Mini
* ConnectCore 8M Mini System-on-Module (SOM)
  * [CC-WMX-ET8D-NN](https://www.digi.com/cc8mmini)
* ConnectCore 8M Mini Development Kit
  * [CC-WMX8MM-KIT](https://www.digi.com/products/models/cc-wmx8mm-kit) ([Get Started](https://www.digi.com/resources/documentation/digidocs/embedded/dey/3.2/cc8mmini/yocto-gs_index))

## ConnectCore 8M Nano
* ConnectCore 8M Nano System-on-Module (SOM)
  * [CC-WMX-FS7D-NN](https://www.digi.com/cc8mnano)
* ConnectCore 8M Nano Development Kit
  * [CC-WMX8MN-KIT](https://www.digi.com/products/models/cc-wmx8mn-kit) ([Get Started](https://www.digi.com/resources/documentation/digidocs/embedded/dey/3.2/cc8mnano/yocto-gs_index))

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
  * [CC-WMX8-PRO](https://www.digi.com/products/embedded-systems/single-board-computers/digi-connectcore-8x-sbc-pro) ([Get Started](https://www.digi.com/resources/documentation/digidocs/embedded/dey/3.2/cc8x/yocto-gs_index))

## ConnectCore 6UL
* ConnectCore 6UL System-on-Module (SOM)
  * [CC-WMX-JN58-NE](https://www.digi.com/products/models/cc-wmx-jn58-ne)
  * [CC-MX-JN58-Z1](https://www.digi.com/products/models/cc-mx-jn58-z1)
  * CC-WMX-JN7A-NE
  * [CC-WMX-JN7A-CBX](https://www.digi.com/products/models/cc-wmx-jn7a-cbx)
  * [CC-WMX-JN68-NN](https://www.digi.com/products/models/cc-wmx-jn68-nn)
  * [CC-WMX-JN69-NN](https://www.digi.com/products/models/cc-wmx-jn69-nn)
  * [CC-MX-JN69-ZN](hhtps://www.digi.com/products/models/cc-mx-jn69-zn)
* ConnectCore 6UL SBC Express
  * [CC-WMX6UL-START](https://www.digi.com/products/models/cc-wmx6ul-start) ([Get Started](https://www.digi.com/resources/documentation/digidocs/embedded/dey/3.2/cc6ul/yocto-gs_index))
  * [CC-SBE-WMX-JN58](https://www.digi.com/products/models/cc-sbe-wmx-jn58)
* ConnectCore 6UL SBC Pro
  * [CC-WMX6UL-KIT](https://www.digi.com/products/models/cc-wmx6ul-kit) ([Get Started](https://www.digi.com/resources/documentation/digidocs/embedded/dey/3.2/cc6ul/yocto-gs_index))
  * [CC-SBP-WMX-JN58](https://www.digi.com/products/models/cc-sbp-wmx-jn58)

## ConnectCore 6 Plus
* ConnectCore 6 Plus System-on-Module (SOM)
  * [CC-WMX-KK8D-TN](https://www.digi.com/products/models/cc-wmx-kk8d-tn)
* ConnectCore 6 Plus professional development kit
  * [CC-WMX6P-KIT](https://www.digi.com/products/models/cc-wmx6p-kit) ([Get Started](https://www.digi.com/resources/documentation/digidocs/embedded/dey/3.2/cc6plus/yocto-gs_index))

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
  * [CC-WMX6-KIT](https://www.digi.com/products/models/cc-wmx6-kit) ([Get Started](https://www.digi.com/resources/documentation/digidocs/embedded/dey/3.2/cc6/yocto-gs_index))
  * [CC-SB-WMX-J97C-1](https://www.digi.com/products/models/cc-sb-wmx-j97c-1)
  * [CC-SB-WMX-L87C-1](https://www.digi.com/products/models/cc-sb-wmx-l87c-1)
  * [CC-SB-WMX-L76C-1](https://www.digi.com/products/models/cc-sb-wmx-l76c-1)

# Installation

Digi Embedded Yocto is composed of a set of different Yocto layers that work in
parallel. The layers are specified on a [manifest](https://github.com/digi-embedded/dey-manifest/blob/gatesgarth/default.xml) file.

To install, please follow the instructions at the dey-manifest [README](https://github.com/digi-embedded/dey-manifest)

# Documentation

Documentation is available online at https://www.digi.com/resources/documentation/digidocs/embedded/

# Downloads

* Demo images: https://ftp1.digi.com/support/digiembeddedyocto/3.2/r2/images/
* Software Development Kit (SDK): https://ftp1.digi.com/support/digiembeddedyocto/3.2/r2/sdk/

# Release Changelog

## 3.2-r2

* Updated GLIBC to v2.33
* Updated busybox to v1.34.1
* Updated OpenSSL to v1.1.1l
* Updated QCA65x4 Wi-Fi firmware
* Updated NXP eIQ revision
* Improved Dual boot support
* Added support to ConnectCore 6N
* Added support to SELinux
* Added support to Read-only root file system
* Other minor fixes.

## 3.2-r1

* Release based on [Yocto 3.2 (Gatesgarth)](https://www.yoctoproject.org/software-overview/downloads) including:
  * New toolchain based on GLIBC-2.32
  * Updated Qt 5.15.2
  * Updated bluez5 to v5.55
  * Updated gstreamer1.0 to v1.16.3
  * Updated OpenSSL to v1.1.1k
  * Updated busybox to v1.32.0
  * Updated NetworkManager to v1.22.14
  * Package upgrades and security fixes
* Updated i.MX8 SCU firmware to v1.7.1.1
* Updated mca-tool to v1.24
* Updated trustfence-tool to v2.5
* Added support for Dual boot mechanism
* Added support for Google Coral AI devices
* Trustfence: added support for generic partition encryption
* MCA: added support for GPIO-refreshed watchdog
* Improved WebKit performance
* Improved installation scripts
* Updated CC6UL RF calibration

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

## ConnectCore 8M Nano

* ConnectCore 8M Nano System-on-Module (SOM)
  * CPU wake-up sources are not yet supported

## ConnectCore 8X

* i.MX8QXP Processor
  * GPU maximum performance reduced. The maximum frequency targets are 850 MHz
    for the shaders and 700 MHz for the core. However, in this hardware release
    the maximum frequency is limited to 650 MHz for the shaders and 600 MHz for
    the core, with the corresponding performance reduction. These targets will
    be met in future releases of the hardware.
  * BSDL operation is not supported. It will be available in future releases
    of the hardware.

## ConnectCore 6UL

* ConnectCore 6UL System-on-Module (SOM)
  * The UART connected to the Bluetooth chip on early versions of the ConnectCore
    6UL system-on-module (hardware version < 4) cannot properly execute flow
    control. To work around this issue, UART1 of these SOM versions has been
    configured to operate at 115200 bps and without hardware flow control,
    reducing the maximum throughput of this interface.
  * The QCA6564 wireless chip does not support Wake On Wireless LAN.

## ConnectCore 6 Plus

* ConnectCore 6 Plus System-on-Module (SOM)
  * NXP i.MX6QP processor has a documented errata (ERR004512) whereby the maximum
    performance of the Gigabit FEC is limited to 400Mbps (total for Tx and Rx).
* ConnectCore 6 Plus SBC
  * The Micrel PHY KSZ9031 may take between five and six seconds to
    auto-negotiate with Gigabit switches.

## ConnectCore 6

* ConnectCore 6 System-on-Module (SOM)
  * NXP i.MX6 processor has a documented errata (ERR004512) whereby the maximum
    performance of the Gigabit FEC is limited to 400Mbps (total for Tx and Rx).
  * When using softAP mode on Band A on the Qualcomm AR6233, channels used for
    Dynamic Frequency Selection (DFS) are not supported.
  * The Qualcomm AR6233 firmware does not support the following configuration
    modes:
    * Concurrent modes involving P2P mode, such as P2P + softAP or P2P + STA.
    * Bluetooth + softAP + STA concurrent mode.
  * A maximum of five clients are supported when using Qualcomm's AR6233 in
    softAP mode.
  * A maximum of ten connected devices are supported when using Qualcomm's AR6233
    Bluetooth Low Energy mode.
* ConnectCore 6 SBC
  * The Micrel PHY KSZ9031 may take between five and six seconds to
    auto-negotiate with Gigabit switches.

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

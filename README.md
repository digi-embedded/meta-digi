# Digi Embedded Yocto (DEY) 2.4
## Release 2.4-r2-beta

This document provides information about Digi Embedded Yocto,
Digi International's professional embedded Yocto development environment.

Digi Embedded Yocto 2.4 is based on the Yocto Project(TM) 2.4 (Rocko) release.

This is a beta release for the ConnectCore 8X SBC Express. The rest of the
platforms are supported but have not been fully tested.

For a full list of supported features and interfaces please refer to the
online documentation.

# Tested OS versions

The current release has been verified and tested with the following
OS versions:

* Ubuntu 16.04

# Supported Platforms

Software for the following hardware platforms is in production support:

## ConnectCore 6UL
* ConnectCore 6UL System-on-Module (SOM)
  * [CC-WMX-JN58-NE](https://www.digi.com/products/models/cc-wmx-jn58-ne)
  * [CC-MX-JN58-Z1](https://www.digi.com/products/models/cc-mx-jn58-z1)
  * CC-WMX-JN7A-NE
* ConnectCore 6UL SBC Express
  * [CC-WMX6UL-START](https://www.digi.com/products/models/cc-wmx6ul-start) ([Get Started](https://www.digi.com/resources/documentation/digidocs/90001548/default.htm#concept/yocto/c_get_started_with_yocto.htm))
  * [CC-SBE-WMX-JN58](https://www.digi.com/products/models/cc-sbe-wmx-jn58)
* ConnectCore 6UL SBC Pro
  * [CC-WMX6UL-KIT](https://www.digi.com/products/models/cc-wmx6ul-kit) ([Get Started](https://www.digi.com/resources/documentation/digidocs/90001547/default.htm#concept/yocto/c_get_started_with_yocto.htm))
  * [CC-SBP-WMX-JN58](https://www.digi.com/products/models/cc-sbp-wmx-jn58)

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
* ConnectCore 6 SBC
  * [CC-WMX6-KIT](https://www.digi.com/products/models/cc-wmx6-kit) ([Get Started](https://www.digi.com/resources/documentation/digidocs/90001546/default.htm#concept/yocto/c_get_started_with_yocto.htm))
  * [CC-SB-WMX-J97C-1](https://www.digi.com/products/models/cc-sb-wmx-j97c-1)
  * [CC-SB-WMX-L87C-1](https://www.digi.com/products/models/cc-sb-wmx-l87c-1)
  * [CC-SB-WMX-L76C-1](https://www.digi.com/products/models/cc-sb-wmx-l76c-1)

## ConnectCore 6 Plus
* ConnectCore 6 Plus System-on-Module (SOM)
  * CC-WMX-KK8D-TN
* ConnectCore 6 Plus SBC
  * CC-SB-WMX-KK8D

## ConnectCore 8X
* ConnectCore 8X System-on-Module (SOM)
  * [CC-WMX-JM7D-NN](https://www.digi.com/cc8x)
  * [CC-WMX-JM8E-NN](https://www.digi.com/cc8x)
  * [CC-MX-JM8D-ZN](https://www.digi.com/cc8x)
  * [CC-WMX-JN7D-NN](https://www.digi.com/cc8x)
  * [CC-MX-JN7D-ZN](https://www.digi.com/cc8x)
* ConnectCore 8X SBC Express
  * [CC-WMX8-EXPRESS](https://www.digi.com/cc8x)

Previous versions of Digi Embedded Yocto include support for additional Digi
hardware.

# Installation

Digi Embedded Yocto is composed of a set of different Yocto layers that work in
parallel. The layers are specified on a [manifest](https://github.com/digi-embedded/dey-manifest/blob/rocko/default.xml) file.

To install, please follow the instructions at the dey-manifest [README](https://github.com/digi-embedded/dey-manifest)

# Documentation

Documentation is available online on the Digi documentation site:

* [ConnectCore 6UL SBC Express](https://www.digi.com/resources/documentation/digidocs/90001548/default.htm)
* [ConnectCore 6UL SBC Pro](https://www.digi.com/resources/documentation/digidocs/90001547/default.htm)
* [ConnectCore 6 SBC](https://www.digi.com/resources/documentation/digidocs/90001546/default.htm)
* [ConnectCore 6 Plus SBC](https://www.digi.com/resources/documentation/digidocs/90002275/default.htm)
* [ConnectCore 8X SBC Express](https://www.digi.com/cc8x/documentation)

# Downloads

* Demo images: ftp://ftp1.digi.com/support/digiembeddedyocto/2.4/r2-beta/images/
* Software Development Kit (SDK): ftp://ftp1.digi.com/support/digiembeddedyocto/2.4/r2-beta/sdk/

# Release Changelog

## 2.4-r2-beta

* Added support for ConnectCore 8X platform. Supported features:
    * Bluetooth and Wireless interfaces (STA, SoftAP, P2P, Concurrence)
    * MCA: Reset, Watchdog, RTC, GPIOs and ADCs
    * SBC Express interfaces: Ethernet, microSD, UART, I2C, CAN, SPI, USB host,
      USB device, GPIOs, XBee, LVDS Display, Touch Screen controller
    * Graphics and Video
* Updated U-Boot to version 2017.03-r1
* Fixed XBee RESET_N polarity assertion. Requires updating polarity of the
``digi,reset-gpio`` property of node ``xbee`` on device tree files derived from
ConnectCore 6UL SBC Pro to use polarity **GPIO_ACTIVE_LOW**

## 2.4-r1

* Release based on [Yocto 2.4 (Rocko)](https://www.yoctoproject.org/downloads/core/rocko24) including:
  * New toolchain based on GCC-7.2.0 and GLIBC-2.26
  * Updated Qt 5.9.4
  * Package upgrades and security fixes
* Updated kernel version to v4.9 for ConnectCore 6 platform
* Updated U-Boot to version 2015.04-r12
* Updated Cryptoauthlib version to v3.0
* Added support for Goodix 10" display
* Updated documentation

# Known Issues and Limitations

This is a list of known issues and limitations at the time of release. An
updated list can be found on the online documentation.

* If TrustFence (TM) image encryption support is enabled, the uSD image will
boot a signed U-Boot only.
* Firmware update
  * The software update package must be located in the root level of the
    update media (subfolders are not yet supported).
* Cloud Connector
  * Remote file system management fails with long file names and paths
    (over 255 characters).
* For P2P connections Digi recommends "Negotiated GO" modes. The QCA6564
  devices (ConnectCore 6UL and ConnectCore 6 Plus) running a 4.9 kernel
  version fail to join to autonomous groups.

## ConnectCore 6UL

* ConnectCore 6UL System-on-Module (SOM)
  * The UART connected to the Bluetooth chip on early versions of the ConnectCore
    6UL system-on-module (hardware version < 4) cannot properly execute flow
    control. To work around this issue, UART1 of these SOM versions has been
    configured to operate at 115200 bps and without hardware flow control,
    reducing the maximum throughput of this interface.
  * When using wireless concurrent mode as the wireless interface is shared
    between several different functionalities performance is reduced.
  * When using wireless concurrent mode Digi recommends to keep the different
    modes on the same frequency channels. For example, when configuring access
    point mode on channel 36 on band A, the recommendation would be to connect
    to the same channel both in station mode and WiFi direct so that the radio
    performance is optimized.
  * When working as an access point, DFS capable channels in band A are not
    currently supported.
  * The QCA6564 wireless chip does not support Wake On Wireless LAN.

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

## ConnectCore 6 Plus

* ConnectCore 6 Plus System-on-Module (SOM)
  * NXP i.MX6QP processor has a documented errata (ERR004512) whereby the maximum
    performance of the Gigabit FEC is limited to 400Mbps (total for Tx and Rx).
* ConnectCore 6 Plus SBC
  * The Micrel PHY KSZ9031 may take between five and six seconds to
    auto-negotiate with Gigabit switches.

## ConnectCore 8X

* i.MX8QXP Processor
  * The following processor features are not supported in this BSP release:
    Power Management, PWM, ADC, RS485. They will be implemented in future releases.
  * VPU unit is not fully functional, hence hardware video encoding/decoding is
    not available. On this release, video encoding/decoding can be done via
    software plugins, reducing the performance of video playback. This will be
    available in future releases of the hardware.
  * GPU maximum performance reduced. The maximum frequency targets are 850 MHz
    for the shaders and 700 MHz for the core. However, in this hardware release
    the maximum frequency is limited to 650 MHz for the shaders and 600 MHz for
    the core, with the corresponding performance reduction. These targets will
    be met in future releases of the hardware.
  * BSDL operation is not supported. It will be available in future releases
    of the hardware.
* ConnectCore 8X System-on-Module (SOM)
  * The UART connected to the Bluetooth chip on early versions of the ConnectCore
    8X System-on-Module (hardware version = 1) cannot properly execute flow
    control. To work around this issue, UART1 of these SOM versions has been
    configured to operate at 115200 bps and without hardware flow control,
    reducing the maximum throughput of this interface. It will be fixed in
    future releases of the hardware.
  * MCA firmware cannot be updated without a debugger on early versions of the
    ConnectCore 8X System-on-Module (hardware version = 1). It will be fixed in
    future releases of the hardware.
  * MCA internal voltage reference cannot be enabled on early versions of the
    ConnectCore 8X System-on-Module (hardware version = 1). It will be fixed in
    future releases of the hardware.
  * MCA tamper pins configuration is not supported. It will be available in
    future releases.
  * Trustfence (TM) is not supported. It will be available in future releases.

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

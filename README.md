# Digi Embedded Yocto (DEY) 2.2
## Release 2.2-r1

This document provides information about Digi Embedded Yocto,
Digi International's professional embedded Yocto development environment.

Digi Embedded Yocto 2.2 is based on the 2.2 (Morty) Yocto release.

For a full list of supported features and interfaces please refer to the
online documentation.

# Supported Platforms

The current release supports the following hardware platforms:

Software for the following hardware platforms is in production support:

* Digi ConnectCore 6UL
  * [Digi P/N CC-WMX-JN58-NE](http://www.digi.com/products/models/cc-wmx-jn58-ne)
* Digi ConnectCore 6UL SBC Express
  * [Digi P/N CC-WMX6UL-START](http://www.digi.com/products/models/cc-wmx6ul-start) ([Get Started](https://www.digi.com/resources/documentation/digidocs/90001514/default.htm#concept/yocto/c_get_started_with_yocto.htm))
* Digi ConnectCore 6UL SBC Pro
  * [Digi P/N CC-WMX6UL-KIT](https://www.digi.com/products/models/cc-wmx6ul-kit) ([Get Started](https://www.digi.com/resources/documentation/digidocs/90001515/default.htm#concept/yocto/c_get_started_with_yocto.htm))

* Digi ConnectCore 6
  * [Digi P/N CC-WMX-J97C-TN](http://www.digi.com/products/models/cc-wmx-j97c-tn)
  * [Digi P/N CC-WMX-L96C-TE](http://www.digi.com/products/models/cc-wmx-l96c-te)
  * [Digi P/N CC-WMX-L87C-TE](http://www.digi.com/products/models/cc-wmx-l87c-te)
  * [Digi P/N CC-MX-L76C-Z1](http://www.digi.com/products/models/cc-mx-l76c-z1)
  * [Digi P/N CC-MX-L86C-Z1](http://www.digi.com/products/models/cc-mx-l86c-z1)
  * [Digi P/N CC-MX-L96C-Z1](http://www.digi.com/products/models/cc-mx-l96c-z1)
  * [Digi P/N CC-WMX-L76C-TE](http://www.digi.com/products/models/cc-wmx-l76c-te)
  * Digi P/N CC-WMX-K87C-FJA
  * Digi P/N CC-WMX-K77C-TE
  * Digi P/N CC-WMX-L97D-TN
  * Digi P/N CC-WMX-J98C-FJA
  * Digi P/N CC-WMX-J98C-FJA-1

* Digi ConnectCore 6 Development Kit
  * [Digi P/N CC-WMX6-KIT](http://www.digi.com/products/models/cc-wmx6-kit) ([Get Started](http://www.digi.com/resources/documentation/digidocs/90001945-13/default.htm#concept/yocto/c_get_started_with_yocto.htm%3FTocPath%3DDigi%2520Embedded%2520Yocto%7CGet%2520started%7C_____0))

* Digi ConnectCore 6 SBC
  * [Digi P/N CC-SB-WMX-J97C-1](http://www.digi.com/products/models/cc-sb-wmx-j97c-1)
  * [Digi P/N CC-SB-WMX-L87C-1](https://www.digi.com/products/models/cc-sb-wmx-l87c-1)
  * [Digi P/N CC-SB-WMX-L76C-1](https://www.digi.com/products/models/cc-sb-wmx-l76c-1)

Previous versions of Digi Embedded Yocto include support for additional Digi
hardware.

# Documentation

Documentation is available online on the Digi documentation site:

* [Digi ConnectCore 6UL SBC Express](http://www.digi.com/resources/documentation/Digidocs/90001514/default.htm)
* [Digi ConnectCore 6UL SBC Pro](http://www.digi.com/resources/documentation/Digidocs/90001515/default.htm)
* [Digi ConnectCore 6 Jumpstart Development Kit](http://www.digi.com/resources/documentation/Digidocs/90001945-13/default.htm)

# Downloads

* Demo images: ftp://ftp1.digi.com/support/digiembeddedyocto/2.2/r1/images/
* Software Development Kit (SDK): ftp://ftp1.digi.com/support/digiembeddedyocto/2.2/r1/sdk/

# Release Changelog

## 2.2-r1

* Release based on [Yocto 2.2 (Morty)](https://www.yoctoproject.org/downloads/core/morty22) including:
  * New toolchain based on GCC-6.2.0 and GLIBC-2.24
  * Updated Qt 5.7
  * Updated ModemManager with validated support for:
    * Digi's XBee Cellular LTE Cat 1 (USA/Verizon), with P/N XBC-V1-UT-001
    * Telit's LE910 and HE910
    * Huawei's ME909u
    * Quectel's EC21
  * Modified default networking settings:
    * Defalt to dynamic IP addresses assignments
    * Default station and softAP concurrent wireless mode
  * TrustFence enabled
  * Remote manager
  * Local and remote manager firmware update
  * Package upgrades and security fixes
  * U-boot support for 1GB DDR3 RAM on CC6UL

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

## Digi ConnectCore 6UL

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
  performance is optimized
* When working as an access point, DFS capable channels in band A are not
  currently supported.

## Digi ConnectCore 6

* NXP i.MX6 processor has a documented errata (ERR004512) whereby the maximum
performance of the Gigabit FEC is limited to 400Mbps (total for Tx and Rx)
* When using softAP mode on Band A on the Qualcomm AR6233, channels used for
Dynamic Frequency Selection (DFS) are not supported
* The Qualcomm AR6233 firmware does not support the following configuration
modes:
  * Concurrent modes involving P2P mode, such as P2P + softAP or P2P + STA
  * Bluetooth + softAP + STA concurrent mode
* A maximum of five clients are supported when using Qualcomm's AR6233 in
softAP mode
* A maximum of ten connected devices are supported when using Qualcomm's AR6233
Bluetooth Low Energy mode

## Digi ConnectCore 6 SBC

* The Micrel PHY KSZ9031 may take between five and six seconds to
auto-negotiate with Gigabit switches

# Support Contact Information

For support questions please contact Digi Technical Support:

* [Enterprise Support](https://mydigi.secure.force.com/customers/)
* [Product Technical Support](http://www.digi.com/support/product-support)
* [Support forum](http://www.digi.com/support/forum/)

When you contact Digi Technical Support, include important system details and
device information to help Digi resolve the issue more quickly.

1. In the device, run the command 'sysinfo'. This generates the following file:
   /tmp/<current timestamp>.txt.gz.
2. Attach the <current timestamp>.txt.gz file to your support ticket.

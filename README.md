# Digi Embedded Yocto (DEY) 2.6
## Release 2.6-r1.2

This document provides information about Digi Embedded Yocto,
Digi International's professional embedded Yocto development environment.

Digi Embedded Yocto 2.6 is based on the Yocto Project(TM) 2.6 (Thud) release.

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

## ConnectCore 8X
* ConnectCore 8X System-on-Module (SOM)
  * [CC-WMX-JM7D-NN](https://www.digi.com/cc8x)
  * [CC-WMX-JM8E-NN](https://www.digi.com/cc8x)
  * [CC-MX-JM8D-ZN](https://www.digi.com/cc8x)
  * [CC-WMX-JN7D-NN](https://www.digi.com/cc8x)
  * [CC-MX-JN7D-ZN](https://www.digi.com/cc8x)
* ConnectCore 8X SBC Express
  * [CC-WMX8-EXPRESS](https://www.digi.com/cc8x)
* ConnectCore 8X SBC Pro
  * [CC-WMX8-PRO](https://www.digi.com/cc8x)

Previous versions of Digi Embedded Yocto include support for additional Digi
hardware.

# Installation

Digi Embedded Yocto is composed of a set of different Yocto layers that work in
parallel. The layers are specified on a [manifest](https://github.com/digi-embedded/dey-manifest/blob/thud/default.xml) file.

To install, please follow the instructions at the dey-manifest [README](https://github.com/digi-embedded/dey-manifest)

# Documentation

Documentation is available online at https://www.digi.com/resources/documentation/digidocs/embedded/

# Downloads

* Demo images: ftp://ftp1.digi.com/support/digiembeddedyocto/2.6/r1/images/
* Software Development Kit (SDK): ftp://ftp1.digi.com/support/digiembeddedyocto/2.6/r1/sdk/

# Release Changelog

## 2.6-r1

* Release based on [Yocto 2.6 (Thud)](https://www.yoctoproject.org/software-overview/downloads) including:
  * New toolchain based on GLIBC-2.28
  * Updated Qt 5.11.3
  * Updated NetworkManager to v1.14.4
  * Updated Wpa-supplicant to v2.6
  * Updated gstreamer1.0 to v1.14.0
  * Updated busybox to v1.29.2
  * Updated bluez5 to v5.50
  * Updated OpenSSL to v1.1.1a
  * Package upgrades and security fixes
* Updated kernel version to v4.14.111 for i.MX8X and i.MX6UL platforms
* Updated U-Boot to version 2018.03-r1 for i.MX8X platform
* Updated AWS Greengrass core to version 1.8.0
* Added support for Code Signing Tool 3.1.0
* Changed initialization manager in ConnectCore 8X platforms to systemd

# Known Issues and Limitations

This is a list of known issues and limitations at the time of release. An
updated list can be found on the online documentation.

* Firmware update
  * The software update package must be located in the root level of the
    update media (subfolders are not yet supported).
* Cloud Connector
  * Remote file system management fails with long file names and paths
    (over 255 characters).
* For P2P connections Digi recommends "Negotiated GO" modes. The QCA6564
  devices (ConnectCore 6UL) running a 4.14 kernel version fail to join to
  autonomous groups.
* Disabling Trustfence secure console via GPIO is not supported on U-Boot v2017.03.
* Trustfence is not yet supported on U-Boot v2018.03.

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

## ConnectCore 8X

* i.MX8QXP Processor
  * GPU maximum performance reduced. The maximum frequency targets are 850 MHz
    for the shaders and 700 MHz for the core. However, in this hardware release
    the maximum frequency is limited to 650 MHz for the shaders and 600 MHz for
    the core, with the corresponding performance reduction. These targets will
    be met in future releases of the hardware.
  * BSDL operation is not supported. It will be available in future releases
    of the hardware.
* Digi Embedded Yocto
  * The following features are not supported in this release for the ConnectCore 8X platform:
    * Trustfence (TM)
    * Digi Remote Manager

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

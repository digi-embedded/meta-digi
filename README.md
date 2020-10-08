# Digi Embedded Yocto (DEY) 3.0
## Release 3.0-r2

This document provides information about Digi Embedded Yocto,
Digi International's professional embedded Yocto development environment.

Digi Embedded Yocto 3.0 is based on the Yocto Project(TM) 3.0 (Zeus) release.

For a full list of supported features and interfaces please refer to the
online documentation.

# Tested OS versions

The current release has been verified and tested with the following
OS versions:

* Ubuntu 18.04

# Supported Platforms

Software for the following hardware platforms is in production support:

## ConnectCore 8M Nano
* ConnectCore 8M Nano System-on-Module (SOM)
  * [CC-WMX-FS7D-NN](https://www.digi.com/cc8mnano)
* ConnectCore 8M Nano Development Kit
  * [CC-WMX8MN-KIT](https://www.digi.com/products/models/cc-wmx8mn-kit) ([Get Started](https://www.digi.com/resources/documentation/digidocs/embedded/dey/3.0/cc8mnano/yocto-gs_index))

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
  * [CC-WMX8-PRO](https://www.digi.com/products/embedded-systems/single-board-computers/digi-connectcore-8x-sbc-pro) ([Get Started](https://www.digi.com/resources/documentation/digidocs/embedded/dey/3.0/cc8x/yocto-gs_index))

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
  * [CC-WMX6UL-START](https://www.digi.com/products/models/cc-wmx6ul-start) ([Get Started](https://www.digi.com/resources/documentation/digidocs/embedded/dey/3.0/cc6ul/yocto-gs_index))
  * [CC-SBE-WMX-JN58](https://www.digi.com/products/models/cc-sbe-wmx-jn58)
* ConnectCore 6UL SBC Pro
  * [CC-WMX6UL-KIT](https://www.digi.com/products/models/cc-wmx6ul-kit) ([Get Started](https://www.digi.com/resources/documentation/digidocs/embedded/dey/3.0/cc6ul/yocto-gs_index))
  * [CC-SBP-WMX-JN58](https://www.digi.com/products/models/cc-sbp-wmx-jn58)

Previous versions of Digi Embedded Yocto include support for additional Digi
hardware.

# Installation

Digi Embedded Yocto is composed of a set of different Yocto layers that work in
parallel. The layers are specified on a [manifest](https://github.com/digi-embedded/dey-manifest/blob/zeus/default.xml) file.

To install, please follow the instructions at the dey-manifest [README](https://github.com/digi-embedded/dey-manifest)

# Documentation

Documentation is available online at https://www.digi.com/resources/documentation/digidocs/embedded/

# Downloads

* Demo images: https://ftp1.digi.com/support/digiembeddedyocto/3.0/r1/images/
* Software Development Kit (SDK): https://ftp1.digi.com/support/digiembeddedyocto/3.0/r1/sdk/

# Release Changelog

## 3.0-r1

* Release based on [Yocto 3.0 (Zeus)](https://www.yoctoproject.org/software-overview/downloads) including:
  * New toolchain based on GLIBC-2.30
  * Updated Qt 5.13.2
  * Updated NetworkManager to v1.18.4
  * Updated Wpa-supplicant to v2.9
  * Updated gstreamer1.0 to v1.16.0
  * Updated busybox to v1.31.0
  * Removed support to bluez4
  * Updated OpenSSL to v1.1.1d
  * Package upgrades and security fixes
* Updated kernel version to v5.4 for ConnectCore 8X, ConnecCore 8M Nano and
  ConnectCore 6UL
* Updated U-Boot to version 2017.03-r6 for ConnectCore 6UL
* Updated U-Boot to version 2018.03-r4 for ConnectCore 8 MNano platform
* Updated U-Boot to version 2019.04-r2 for ConnectCore 8X platform
* Added support for ConnectCore 8M Nano v2 SOM
* Added initial TrustFence support for ConnectCore 8M Nano

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
    devices (ConnectCore 6UL and ConnectCore 8M Nano) fail to join autonomous
    groups.
* Trustfence is not yet supported on the ConnectCore 8M Nano.

## ConnectCore 8M Nano

* ConnectCore 8M Nano System-on-Module (SOM)
  * CPU wake-up sources are not yet supported
* ConnectCore 8M Nano DVK
  * The maximum bitrate for CAN interface is 125 Kbits/s. This is a software
  limitation from the CAN controller.

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

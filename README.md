# Digi Embedded Yocto (DEY) 4.0
## Release 4.0-r1

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

## ConnectCore MP15
* ConnectCore MP15 System-on-Module (SOM)
  * [CC-WST-DW69-NM](https://www.digi.com/products/models/cc-wst-dw69-nm)
  * [CC-ST-DW69-ZM](https://www.digi.com/products/models/cc-st-dw69-zm)
* ConnectCore MP15 DVK
  * [CC-WMP157-KIT](https://www.digi.com/products/models/cc-wmp157-kit) ([Get Started](https://www.digi.com/resources/documentation/digidocs/embedded/dey/4.0/ccmp15/yocto-gs_index))

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
  * [CC-WMX6UL-START](https://www.digi.com/products/models/cc-wmx6ul-start) ([Get Started](https://www.digi.com/resources/documentation/digidocs/embedded/dey/4.0/cc6ul/yocto-gs_index))
  * [CC-SBE-WMX-JN58](https://www.digi.com/products/models/cc-sbe-wmx-jn58)
* ConnectCore 6UL SBC Pro
  * [CC-WMX6UL-KIT](https://www.digi.com/products/models/cc-wmx6ul-kit) ([Get Started](https://www.digi.com/resources/documentation/digidocs/embedded/dey/4.0/cc6ul/yocto-gs_index))
  * [CC-SBP-WMX-JN58](https://www.digi.com/products/models/cc-sbp-wmx-jn58)

# Installation

Digi Embedded Yocto is composed of a set of different Yocto layers that work in
parallel. The layers are specified on a [manifest](https://github.com/digi-embedded/dey-manifest/blob/kirkstone/default.xml) file.

To install, please follow the instructions at the dey-manifest [README](https://github.com/digi-embedded/dey-manifest)

# Documentation

Documentation is available online at https://www.digi.com/resources/documentation/digidocs/embedded/

# Downloads

* Demo images: https://ftp1.digi.com/support/digiembeddedyocto/4.0/r1/images/
* Software Development Kit (SDK): https://ftp1.digi.com/support/digiembeddedyocto/4.0/r1/sdk/

# Release Changelog

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

## ConnectCore MP15

* ConnectCore MP15 System-on-Module (SOM)
  * Power management:
    * Audio interface does not work after suspend.

  * The following features are not yet supported:
    * TrustFence

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

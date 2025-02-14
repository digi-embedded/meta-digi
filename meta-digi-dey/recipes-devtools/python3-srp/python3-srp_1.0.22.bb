# Copyright (C) 2022-2025, Digi International Inc.

SUMMARY = "Library providing an implementation of the Secure Remote Password protocol (SRP)"
DESCRIPTION = "SRP is a cryptographically strong authentication protocol for password-based, mutual authentication over an insecure network connection."
HOMEPAGE = "https://github.com/cocagne/pysrp"
SECTION = "devel/python"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=df47fd55f4b03bc3a3616c1b4e6187a4"

SRC_URI[sha256sum] = "f330d0ec7387e2ac8577487b164963155d4a031bca6e2024f1b0930eb92baa5d"

inherit setuptools3_legacy pypi

RDEPENDS:${PN} += "python3-six"
RPROVIDES:${PN} = "python3-srp"

BBCLASSEXTEND = "native nativesdk"

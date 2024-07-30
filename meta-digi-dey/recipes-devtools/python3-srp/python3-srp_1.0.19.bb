# Copyright (C) 2022, Digi International Inc.

SUMMARY = "Library providing an implementation of the Secure Remote Password protocol (SRP)"
DESCRIPTION = "SRP is a cryptographically strong authentication protocol for password-based, mutual authentication over an insecure network connection."
HOMEPAGE = "https://github.com/cocagne/pysrp"
SECTION = "devel/python"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=df47fd55f4b03bc3a3616c1b4e6187a4"

SRC_URI[sha256sum] = "48e653e8c3f590909ba407306eb2e8460a2c7d1f86c56bce59cf42af54ff5d2a"

inherit setuptools3_legacy pypi

RDEPENDS:${PN} += "python3-six"
RPROVIDES:${PN} = "python3-srp"

BBCLASSEXTEND = "native nativesdk"

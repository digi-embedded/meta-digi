DESCRIPTION = "Python library to create SVG drawings"
SECTION = "devel/python"
HOMEPAGE = "https://github.com/mozman/svgwrite"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE.TXT;md5=3e14f2d1a8674ddcbbd8b51762250049"

inherit pypi setuptools3

PYPI_PACKAGE = "svgwrite"
PYPI_PACKAGE_EXT = "zip"

SRC_URI[md5sum] = "6132f0d8611ac0d5a8a8731636aa03f8"
SRC_URI[sha256sum] = "e220a4bf189e7e214a55e8a11421d152b5b6fb1dd660c86a8b6b61fe8cc2ac48"

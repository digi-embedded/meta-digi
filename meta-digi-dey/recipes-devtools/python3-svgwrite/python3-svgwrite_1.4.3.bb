DESCRIPTION = "Python library to create SVG drawings"
SECTION = "devel/python"
HOMEPAGE = "https://github.com/mozman/svgwrite"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE.TXT;md5=3e14f2d1a8674ddcbbd8b51762250049"

inherit pypi setuptools3

PYPI_PACKAGE = "svgwrite"
PYPI_PACKAGE_EXT = "zip"

SRC_URI[md5sum] = "8e6d536bdffefa03341b77dff5add485"
SRC_URI[sha256sum] = "a8fbdfd4443302a6619a7f76bc937fc683daf2628d9b737c891ec08b8ce524c3"

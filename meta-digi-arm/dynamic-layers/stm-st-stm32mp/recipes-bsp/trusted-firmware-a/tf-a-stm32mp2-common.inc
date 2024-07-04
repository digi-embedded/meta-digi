FILESEXTRAPATHS:prepend := "${THISDIR}/tf-a-stm32mp:"

SECTION = "bootloaders"

LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://license.rst;md5=1dd070c98a281d18d9eefd938729b031"
CVE_PRODUCT = "arm:trusted_firmware-a"

SRC_URI = "git://git.trustedfirmware.org/TF-A/trusted-firmware-a.git;protocol=http;branch=lts-v2.8"
SRCREV = "f94d6db9b101d3d4cd053e54edd5b876f1cc84ec"

SRC_URI += " \
    file://tf-a-st-ddr.tar.gz;subdir=git;name=fw \
    file://0001-v2.8-stm32mp25-beta.patch \
    "

SRC_URI[fw.sha256sum] = "c87d8a03a8feab1f8a51818a7942deade5d31abb7f4afaa6d6dfa922383e9805"

TF_A_VERSION = "v2.8.12"
TF_A_SUBVERSION = "stm32mp"
TF_A_RELEASE = "beta-r1"
PV = "${TF_A_VERSION}-${TF_A_SUBVERSION}-${TF_A_RELEASE}"

ARCHIVER_ST_BRANCH = "${TF_A_VERSION}-${TF_A_SUBVERSION}"
ARCHIVER_ST_REVISION = "${PV}"
ARCHIVER_COMMUNITY_BRANCH = "master"
ARCHIVER_COMMUNITY_REVISION = "${TF_A_VERSION}"

S = "${WORKDIR}/git"

# ---------------------------------
# Configure default preference to manage dynamic selection between tarball and github
# ---------------------------------
STM32MP_SOURCE_SELECTION ?= "tarball"

DEFAULT_PREFERENCE = "${@bb.utils.contains('STM32MP_SOURCE_SELECTION', 'github', '-1', '1', d)}"

SUMMARY = "OPTEE TA development kit for stm32mp"
LICENSE = "BSD-2-Clause & BSD-3-Clause"
LIC_FILES_CHKSUM = "file://LICENSE;md5=c1f21c4f72f372ef38a5a4aee55ec173"

# Select internal or Github OPTEE repo
OPTEE_URI_STASH = "${DIGI_MTK_GIT}/emp/optee_os.git;protocol=ssh"
OPTEE_URI_GITHUB = "${DIGI_GITHUB_GIT}/optee_os.git;protocol=https"
OPTEE_GIT_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1' , '${OPTEE_URI_STASH}', '${OPTEE_URI_GITHUB}', d)}"

SRCBRANCH = "3.19.0/stm/maint_ccmp2-beta"
SRCREV = "be32a34d4c2b6c916a17afc956289630992c68e0"

SRC_URI = " \
    ${OPTEE_GIT_URI};nobranch=1;name=os \
    file://fonts.tar.gz;subdir=git;name=fonts \
"

SRC_URI[fonts.sha256sum] = "4941e8bb6d8ac377838e27b214bf43008c496a24a8f897e0b06433988cbd53b2"

OPTEE_VERSION = "3.19.0"
OPTEE_SUBVERSION = "stm32mp"
OPTEE_RELEASE = "beta-r1"

PV = "${OPTEE_VERSION}-${OPTEE_SUBVERSION}-${OPTEE_RELEASE}"

ARCHIVER_ST_BRANCH = "${OPTEE_VERSION}-${OPTEE_SUBVERSION}"
ARCHIVER_ST_REVISION = "${PV}"
ARCHIVER_COMMUNITY_BRANCH = "master"
ARCHIVER_COMMUNITY_REVISION = "${OPTEE_VERSION}"

S = "${WORKDIR}/git"

OPTEEMACHINE ?= "stm32mp1"
OPTEEMACHINE:stm32mp1common = "stm32mp1"
OPTEEMACHINE:stm32mp2common = "stm32mp2"

OPTEEOUTPUTMACHINE ?= "stm32mp1"
OPTEEOUTPUTMACHINE:stm32mp1common = "stm32mp1"
OPTEEOUTPUTMACHINE:stm32mp2common = "stm32mp2"

# Enable OPTEE_DEBUG_TRACE; If set to 0, LOG_LEVEL defaults to 3 on optee code
ST_OPTEE_DEBUG_TRACE = "1"
# Log level
ST_OPTEE_DEBUG_LOG_LEVEL = "0"

# The package is empty but must be generated to avoid apt-get installation issue
ALLOW_EMPTY:${PN} = "1"

require optee-os-stm32mp2-common.inc

# Specific for revA board
EXTRA_OEMAKE_REVA:stm32mp25revabcommon:append = " CFG_STM32MP25x_REVA=y "
EXTRA_OEMAKE += " ${EXTRA_OEMAKE_REVA}"

# ---------------------------------
# Configure archiver use
# ---------------------------------
include ${@oe.utils.ifelse(d.getVar('ST_ARCHIVER_ENABLE') == '1', 'optee-os-stm32mp-archiver.inc','')}

# ---------------------------------
# Configure default preference to manage dynamic selection between tarball and github
# ---------------------------------
STM32MP_SOURCE_SELECTION ?= "tarball"

DEFAULT_PREFERENCE = "${@bb.utils.contains('STM32MP_SOURCE_SELECTION', 'github', '-1', '1', d)}"

COMPATIBLE_MACHINE = "(ccmp2)"

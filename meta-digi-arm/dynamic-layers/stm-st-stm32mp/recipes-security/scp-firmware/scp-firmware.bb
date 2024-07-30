SUMMARY = "SCP Firmware for stm32mp"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://${S}/license.md;md5=ef610a65bfb6d16f79778877cbfd45df"

SRC_URI = "gitsm://github.com/ARM-software/SCP-firmware;protocol=https;nobranch=1"
SRCREV = "0c7236b1851d90124210a0414fd982dc55322c7c"

SRC_URI += " \
	file://0001-2.12-stm32mp-r1.patch \
	file://0001-Correct-git-error.patch \
"

SCPFW_VERSION    = "2.12"
SCPFW_SUBVERSION = "stm32mp"
SCPFW_RELEASE    = "r1"

PV = "${SCPFW_VERSION}-${SCPFW_SUBVERSION}-${SCPFW_RELEASE}"

S = "${WORKDIR}/git"

###################################################################
#inherit scp-firmware

# Enable use of scp-firmware shared folder
STAGING_SCPFW_DIR = "${TMPDIR}/work-shared/${MACHINE}/scp-firmware"

do_compile[depends] += "scp-firmware:do_configure"
###################################################################

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(stm32mpcommon)"

# Do not remove source code, even if rm_work is configured
RM_WORK_EXCLUDE += "${PN}"

# -----------------------------------------------
# Enable use of work-shared folder
# Make sure to move ${S} to STAGING_SCPFW_DIR. We can't just
# create the symlink in advance as the git fetcher can't cope with
# the symlink.
do_unpack[cleandirs] += "${S}"
do_unpack[cleandirs] += "${STAGING_SCPFW_DIR}"
do_clean[cleandirs] += "${S}"
do_clean[cleandirs] += "${STAGING_SCPFW_DIR}"
python do_symlink_scpfirmwaresrc() {
    # Specific part to update devtool-source class
    if bb.data.inherits_class('devtool-source', d):
        # We don't want to move the source to STAGING_SCPFW_DIR here
        if d.getVar('STAGING_SCPFW_DIR', d):
            d.setVar('STAGING_SCPFW_DIR', '${S}')

    # Copy/Paste from kernel class with adaptation to SCPFW var
    s = d.getVar("S")
    if s[-1] == '/':
        # drop trailing slash, so that os.symlink(scpscr, s) doesn't use s as directory name and fail
        s=s[:-1]
    scpscr = d.getVar("STAGING_SCPFW_DIR")
    if s != scpscr:
        bb.utils.mkdirhier(scpscr)
        bb.utils.remove(scpscr, recurse=True)
        if d.getVar("EXTERNALSRC"):
            # With EXTERNALSRC S will not be wiped so we can symlink to it
            os.symlink(s, scpscr)
        else:
            import shutil
            shutil.move(s, scpscr)
            os.symlink(scpscr, s)
}
addtask symlink_scpfirmwaresrc before do_patch do_configure after do_unpack

# ---------------------------------
# Configure archiver use
# ---------------------------------
include ${@oe.utils.ifelse(d.getVar('ST_ARCHIVER_ENABLE') == '1', 'scp-firmware-archiver.inc','')}

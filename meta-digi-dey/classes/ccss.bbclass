###########################################################
#
# classes/ccss.bbclass - ConnectCore Security Services
#
# Generates an SBOM for the CCSS CVE analysis tool
#
# Copyright (C) 2025 Digi International
#
#
# This source is released under the MIT License.
#
###########################################################

inherit vigiles

CCSS_API_VERSION = "0.1"
CCSS_IMAGE_TYPE ?= "dev"
CCSS_ENABLE ?= "1"

python do_ccss_generate_sbom() {
    import json
    import os
    import shutil
    import tempfile

    # Temporary dir to store all files in the SBOM
    ccss_tmp_dir = tempfile.mkdtemp(dir=d.getVar('TOPDIR'))

    try:
        manifest_file = os.path.join(d.getVar('VIGILES_DIR'), d.getVar('VIGILES_MANIFEST_NAME') + d.getVar('VIGILES_MANIFEST_SUFFIX'))
        ccss_kconfig = os.path.join(d.getVar('VIGILES_DIR'),'.'.join([_get_kernel_pf(d), 'config']))
        ccss_uconfig = os.path.join(d.getVar('VIGILES_DIR'),'.'.join([_get_uboot_pf(d), 'config']))

        # Copy Vigiles manifest and kernel/uboot configs if they're available
        if os.path.exists(manifest_file):
            shutil.copy(manifest_file, os.path.join(ccss_tmp_dir, 'manifest.json'), follow_symlinks=True)
        if os.path.exists(ccss_kconfig):
            shutil.copy(ccss_kconfig, os.path.join(ccss_tmp_dir, 'kernel.config'), follow_symlinks=True)
        if os.path.exists(ccss_uconfig):
            shutil.copy(ccss_uconfig, os.path.join(ccss_tmp_dir, 'uboot.config'), follow_symlinks=True)

        dict_out = dict(
                api_version      = d.getVar('CCSS_API_VERSION'),
                date             = d.getVar('DATETIME'),
                has_ccss_patches = bb.utils.contains('BBFILE_COLLECTIONS', 'digi-security', 'y', 'n', d),
                image_type       = d.getVar('CCSS_IMAGE_TYPE'),
                som              = d.getVar('DIGI_SOM'),
                yocto_codename   = d.getVar('DISTRO_CODENAME')
        )

        with open(os.path.join(ccss_tmp_dir, 'config.json'), 'w') as f_out:
            s = json.dumps(dict_out, indent=2, sort_keys=True)
            f_out.write(s)

        # Create .zip file
        shutil.make_archive(os.path.join(d.getVar('TOPDIR'), 'CCSS_' + d.getVar('IMAGE_BASENAME') + '-' + d.getVar('DATETIME')), 'zip', ccss_tmp_dir)
    finally:
        # Remove temporary dir
        bb.utils.remove(ccss_tmp_dir, recurse=True)
}

# Don't execute do_ccss_generate_sbom() unless explicitly enabled for a given
# image
python __anonymous() {
    if d.getVar('CCSS_ENABLE') != '1':
        d.setVarFlag('do_ccss_generate_sbom', 'noexec', '1')
}

addtask do_ccss_generate_sbom after do_image before do_image_complete

# Since do_ccss_generate_sbom() uses the DATETIME variable to create the
# metadata JSON, said variable will expand to different values when the
# function is parsed in different stages of the build (first by the bitbake
# cooker, and then by a worker). This causes metadata/hash mismatch errors due
# to non-deterministic content, so exclude DATETIME from the hash calculation.
do_ccss_generate_sbom[vardepsexclude]="DATETIME"

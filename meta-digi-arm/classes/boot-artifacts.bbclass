# Class for the generation of boot artifacts

# This function returns a list with the RAM_CONFIGS that match the RAM size
# in the list of UBOOT_CONFIG
def get_uboot_ram_combinations(d):
    import re

    types = d.getVar('UBOOT_CONFIG', True) or ""
    ram_configs = d.getVar('RAM_CONFIGS', True) or ""

    # Convert to arrays
    types = types.split(" ")
    ram_configs = ram_configs.split(" ")

    # Obtain the list of RAM_CONFIGS for whose RAM size there is a match
    # in UBOOT_CONFIG
    matches = []
    for type in types:
        ramsize = re.search("([0-9]*[G|M]B)", type).group(1)
        for ramc in ram_configs:
            if ramsize in ramc:
                matches.append(ramc)

    return " ".join(matches)

UBOOT_RAM_COMBINATIONS = "${@get_uboot_ram_combinations(d)}"

# This function returns a list with the bootable artifacts
def get_bootable_artifacts(d):
    import re

    types = d.getVar('UBOOT_CONFIG', True) or ""
    ram_configs = d.getVar('RAM_CONFIGS', True) or ""
    soc_revisions = d.getVar('SOC_REVISIONS', True) or ""
    uboot_prefix = d.getVar('UBOOT_PREFIX', True) or ""
    uboot_suffix = d.getVar('UBOOT_SUFFIX', True) or ""
    atf_types = d.getVar('TF_A_CONFIG', True) or ""
    fip_type = d.getVar('FIP_UBOOT_CONFIG', True) or ""
    atf_boot_modes = ['nand']
    artifacts = []

    # For platforms with a FIP artifact, ignore u-boot artifacts
    if fip_type != "":
        machine = d.getVar('MACHINE', True) or ""
        # Add ATF artifacts
        for t in atf_types.split(" "):
            if t in atf_boot_modes:
                artifacts.append("arm-trusted-firmware/tf-a-%s-%s.stm32" % (machine, t))
        # Add FIP artifact
        artifacts.append("fip/fip-%s-%s.bin" % (machine, fip_type))
        return " ".join(artifacts)

    # For platforms without RAM_CONFIGS, build the artifacts from UBOOT_CONFIG
    if ram_configs == "":
        for t in types.split(" "):
            artifacts.append("%s-%s.%s" % (uboot_prefix, t.replace("_","-"), uboot_suffix))
        return " ".join(artifacts)
    else:
        machine = d.getVar('MACHINE', True) or ""
        ram_combinations = get_uboot_ram_combinations(d)
        if soc_revisions == "":
            for ramc in ram_combinations.split(" "):
                artifacts.append("%s-%s-%s.%s" % (uboot_prefix, machine, ramc, uboot_suffix))
        else:
            for soc_rev in soc_revisions.split(" "):
                for ramc in ram_combinations.split(" "):
                    artifacts.append("%s-%s-%s-%s.%s" % (uboot_prefix, machine, soc_rev, ramc, uboot_suffix))

        return " ".join(artifacts)

BOOTABLE_ARTIFACTS = "${@get_bootable_artifacts(d)}"

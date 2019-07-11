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

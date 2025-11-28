#
# Copyright (C) 2025, Digi International Inc.
#

# Inherit custom DIGI sign class to skip signing tool and key parsing restrictions
inherit sign-stm32mp-digi

# Obtain password to use in m33 generation
# Get password from file using the given key index
do_compile[prefuncs] += "${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', 'set_m33_sign_key', '', d)}"
python set_m33_sign_key() {
    passfile = d.getVar('TRUSTFENCE_COPRO_PASSWORD_FILE')
    if (os.path.isfile(passfile)):
        with open(passfile, "r") as file:
            p = file.read().strip()
            if (p):
                d.setVar('SIGN_COPRO_ECC_PASS_%s' % (d.getVar('STM32MP_SOC_NAME').strip()), p);
}

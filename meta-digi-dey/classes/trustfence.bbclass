# Adds TrustFence configuration
#
# To use it add the following line to conf/local.conf:
#
# INHERIT += "trustfence"
#
# Inheriting this class enables the following default TrustFence configuration:
#
# * Disabled console
#

# Default secure console configuration
TRUSTFENCE_CONSOLE_DISABLE ?= "1"

# Uncomment to enable the console with the specified passphrase
#TRUSTFENCE_CONSOLE_PASSPHRASE_ENABLE = "my_secure_passphrase"

# Alternatively, uncommment to enable the console with the specified GPIO
#TRUSTFENCE_CONSOLE_GPIO_ENABLE = "4"

IMAGE_FEATURES += "dey-trustfence"

UBOOT_EXTRA_CONF = ""

python () {
    import hashlib

    # Secure console configuration
    if d.getVar("TRUSTFENCE_CONSOLE_DISABLE", True):
        d.appendVar("UBOOT_EXTRA_CONF", "CONFIG_CONSOLE_DISABLE=y ")
        if d.getVar("TRUSTFENCE_CONSOLE_PASSPHRASE_ENABLE", True):
            passphrase_hash = hashlib.sha256(d.getVar("TRUSTFENCE_CONSOLE_PASSPHRASE_ENABLE")).hexdigest()
            d.appendVar("UBOOT_EXTRA_CONF", 'CONFIG_CONSOLE_ENABLE_PASSPHRASE=y CONFIG_CONSOLE_ENABLE_PASSPHRASE_KEY=\\"%s\\" ' % passphrase_hash)
        elif d.getVar("TRUSTFENCE_CONSOLE_GPIO_ENABLE", True):
            d.appendVar("UBOOT_EXTRA_CONF", " CONFIG_CONSOLE_ENABLE_GPIO=y CONFIG_CCIMX6SBC_CONSOLE_ENABLE_GPIO_NR=%s " % d.getVar("TRUSTFENCE_CONSOLE_GPIO_ENABLE"))
}

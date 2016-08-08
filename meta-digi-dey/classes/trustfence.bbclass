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
TRUSTFENCE_CONSOLE_DISABLE ?= "0"

# Uncomment to enable the console with the specified passphrase
#TRUSTFENCE_CONSOLE_PASSPHRASE_ENABLE = "my_secure_passphrase"

# Alternatively, uncommment to enable the console with the specified GPIO
#TRUSTFENCE_CONSOLE_GPIO_ENABLE = "4"

# Default secure boot configuration
TRUSTFENCE_SIGN ?= "1"
TRUSTFENCE_SIGN_KEYS_PATH ?= "default"
TRUSTFENCE_DEK_PATH ?= "default"
TRUSTFENCE_ENCRYPT_ENVIRONMENT ?= "1"

# Trustfence initramfs image recipe
TRUSTFENCE_INITRAMFS_IMAGE ?= "dey-image-trustfence-initramfs"

IMAGE_FEATURES += "dey-trustfence"

UBOOT_EXTRA_CONF = ""

python () {
    import binascii
    import hashlib
    import os

    # Secure console configuration
    if (d.getVar("TRUSTFENCE_CONSOLE_DISABLE", True) == "1"):
        d.appendVar("UBOOT_EXTRA_CONF", "CONFIG_CONSOLE_DISABLE=y ")
        if d.getVar("TRUSTFENCE_CONSOLE_PASSPHRASE_ENABLE", True):
            passphrase_hash = hashlib.sha256(d.getVar("TRUSTFENCE_CONSOLE_PASSPHRASE_ENABLE")).hexdigest()
            d.appendVar("UBOOT_EXTRA_CONF", 'CONFIG_CONSOLE_ENABLE_PASSPHRASE=y CONFIG_CONSOLE_ENABLE_PASSPHRASE_KEY=\\"%s\\" ' % passphrase_hash)
        elif d.getVar("TRUSTFENCE_CONSOLE_GPIO_ENABLE", True):
            d.appendVar("UBOOT_EXTRA_CONF", " CONFIG_CONSOLE_ENABLE_GPIO=y CONFIG_CONSOLE_ENABLE_GPIO_NR=%s " % d.getVar("TRUSTFENCE_CONSOLE_GPIO_ENABLE"))

    # Secure boot configuration
    if (d.getVar("TRUSTFENCE_SIGN_KEYS_PATH") == "default"):
        d.setVar("TRUSTFENCE_SIGN_KEYS_PATH", d.getVar("TOPDIR") + "/trustfence");

    if (d.getVar("TRUSTFENCE_DEK_PATH") == "default"):
        d.setVar("TRUSTFENCE_DEK_PATH", d.getVar("TRUSTFENCE_SIGN_KEYS_PATH") + "/dek.bin");
    
    if (d.getVar("TRUSTFENCE_SIGN", True) == "1"):
        d.appendVar("UBOOT_EXTRA_CONF", "CONFIG_SIGN_IMAGE=y ")
        if d.getVar("TRUSTFENCE_SIGN_KEYS_PATH", True):
            d.appendVar("UBOOT_EXTRA_CONF", 'CONFIG_SIGN_KEYS_PATH=\\"%s\\" ' % d.getVar("TRUSTFENCE_SIGN_KEYS_PATH"))
        if d.getVar("TRUSTFENCE_KEY_INDEX", True):
            d.appendVar("UBOOT_EXTRA_CONF", "CONFIG_KEY_INDEX=%s " % d.getVar("TRUSTFENCE_KEY_INDEX"))
        if (d.getVar("TRUSTFENCE_DEK_PATH", True) not in [None, "0"]):
            d.appendVar("UBOOT_EXTRA_CONF", 'CONFIG_DEK_PATH=\\"%s\\" ' % d.getVar("TRUSTFENCE_DEK_PATH"))
    if (d.getVar("TRUSTFENCE_ENCRYPT_ENVIRONMENT", True) == "1"):
        d.appendVar("UBOOT_EXTRA_CONF", 'CONFIG_ENV_AES=y CONFIG_ENV_AES_CAAM_KEY=y')
}

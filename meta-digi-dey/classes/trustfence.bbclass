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

    # Secure boot configuration
    if d.getVar("TRUSTFENCE_CHECK_KERNEL", True):
        d.appendVar("UBOOT_EXTRA_CONF", "CONFIG_SECURE_BOOT=y ")
    if d.getVar("TRUSTFENCE_UBOOT_SIGN", True):
        d.appendVar("UBOOT_EXTRA_CONF", "CONFIG_SIGN_IMAGE=y ")
        if d.getVar("TRUSTFENCE_CST_PATH", True):
            d.appendVar("UBOOT_EXTRA_CONF", 'CONFIG_CST_PATH=\\"%s\\" ' % d.getVar("TRUSTFENCE_CST_PATH"))
        if d.getVar("TRUSTFENCE_CSF_SIZE", True):
            d.appendVar("UBOOT_EXTRA_CONF", "CONFIG_CSF_SIZE=%s " % d.getVar("TRUSTFENCE_CSF_SIZE"))
        if d.getVar("TRUSTFENCE_KEY_INDEX", True):
            d.appendVar("UBOOT_EXTRA_CONF", "CONFIG_KEY_INDEX=%s " % d.getVar("TRUSTFENCE_KEY_INDEX"))
        if d.getVar("TRUSTFENCE_UBOOT_ENCRYPT", True):
            d.appendVar("UBOOT_EXTRA_CONF", "CONFIG_ENCRYPT_IMAGE=y ")
            if d.getVar("TRUSTFENCE_UBOOT_DEK_SIZE", True):
                d.appendVar("UBOOT_EXTRA_CONF", "CONFIG_DEK_SIZE=%s " % d.getVar("TRUSTFENCE_UBOOT_DEK_SIZE"))
    if d.getVar("TRUSTFENCE_UBOOT_ENV_DEK", True):
        d.appendVar("UBOOT_EXTRA_CONF", 'CONFIG_ENV_AES=y CONFIG_ENV_AES_KEY=\\"%s\\"' % d.getVar("TRUSTFENCE_UBOOT_ENV_DEK"))
}

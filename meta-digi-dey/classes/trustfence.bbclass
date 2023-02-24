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
TRUSTFENCE_SRK_REVOKE_MASK ?= "0x0"

# Partition encryption configuration
TRUSTFENCE_ENCRYPT_PARTITIONS ?= "1"
TRUSTFENCE_ENCRYPT_ROOTFS ?= "${@bb.utils.contains("IMAGE_FEATURES", "read-only-rootfs", "0", "1", d)}"

# Read-only rootfs
TRUSTFENCE_READ_ONLY_ROOTFS ?= "${@bb.utils.contains("IMAGE_FEATURES", "read-only-rootfs", "1", "0", d)}"

IMAGE_FEATURES += "dey-trustfence"

python () {
    import binascii
    import hashlib
    import os

    # Secure console configuration
    if (d.getVar("TRUSTFENCE_CONSOLE_DISABLE") == "1"):
        d.appendVar("UBOOT_TF_CONF", "CONFIG_CONSOLE_DISABLE=y ")
        if d.getVar("TRUSTFENCE_CONSOLE_PASSPHRASE_ENABLE"):
            passphrase_hash = hashlib.sha256(d.getVar("TRUSTFENCE_CONSOLE_PASSPHRASE_ENABLE").encode()).hexdigest()
            d.appendVar("UBOOT_TF_CONF", 'CONFIG_CONSOLE_ENABLE_PASSPHRASE=y CONFIG_CONSOLE_ENABLE_PASSPHRASE_KEY="%s" ' % passphrase_hash)
        elif d.getVar("TRUSTFENCE_CONSOLE_GPIO_ENABLE"):
            d.appendVar("UBOOT_TF_CONF", "CONFIG_CONSOLE_ENABLE_GPIO=y CONFIG_CONSOLE_ENABLE_GPIO_NR=%s " % d.getVar("TRUSTFENCE_CONSOLE_GPIO_ENABLE"))

    # Secure boot configuration
    if (d.getVar("TRUSTFENCE_SIGN_KEYS_PATH") == "default"):
        d.setVar("TRUSTFENCE_SIGN_KEYS_PATH", d.getVar("TOPDIR") + "/trustfence");

    if (d.getVar("TRUSTFENCE_DEK_PATH") == "default"):
        d.setVar("TRUSTFENCE_DEK_PATH", d.getVar("TRUSTFENCE_SIGN_KEYS_PATH") + "/dek.bin");

    if (d.getVar("TRUSTFENCE_SIGN") == "1"):
        d.appendVar("UBOOT_TF_CONF", "CONFIG_SIGN_IMAGE=y CONFIG_AUTH_ARTIFACTS=y ")
        if (d.getVar("TRUSTFENCE_READ_ONLY_ROOTFS") == "1"):
            d.appendVar("UBOOT_TF_CONF", "CONFIG_AUTHENTICATE_SQUASHFS_ROOTFS=y ")
        if d.getVar("TRUSTFENCE_SIGN_KEYS_PATH"):
            d.appendVar("UBOOT_TF_CONF", 'CONFIG_SIGN_KEYS_PATH="%s" ' % d.getVar("TRUSTFENCE_SIGN_KEYS_PATH"))
        if (d.getVar("TRUSTFENCE_UNLOCK_KEY_REVOCATION") == "1"):
            d.appendVar("UBOOT_TF_CONF", "CONFIG_UNLOCK_SRK_REVOKE=y ")
        if d.getVar("TRUSTFENCE_KEY_INDEX"):
            d.appendVar("UBOOT_TF_CONF", "CONFIG_KEY_INDEX=%s " % d.getVar("TRUSTFENCE_KEY_INDEX"))
        if (d.getVar("TRUSTFENCE_DEK_PATH") not in [None, "0"]):
            d.appendVar("UBOOT_TF_CONF", 'CONFIG_DEK_PATH="%s" ' % d.getVar("TRUSTFENCE_DEK_PATH"))
        if d.getVar("TRUSTFENCE_SIGN_MODE"):
            d.appendVar("UBOOT_TF_CONF", 'CONFIG_SIGN_MODE="%s" ' % d.getVar("TRUSTFENCE_SIGN_MODE"))
    if (d.getVar("TRUSTFENCE_ENCRYPT_ENVIRONMENT") == "1"):
        d.appendVar("UBOOT_TF_CONF", "CONFIG_ENV_AES=y CONFIG_ENV_AES_CAAM_KEY=y ")

    # Provide sane default values for SWUPDATE class in case Trustfence is enabled
    if (d.getVar("TRUSTFENCE_SIGN") == "1"):
        # Enable package signing.
        d.setVar("SWUPDATE_SIGNING", "RSA")

        # Retrieve the keys path to use.
        keys_path = d.getVar("TRUSTFENCE_SIGN_KEYS_PATH")

        # Retrieve the key index to use.
        key_index = 0
        if (d.getVar("TRUSTFENCE_KEY_INDEX")):
            key_index = int(d.getVar("TRUSTFENCE_KEY_INDEX"))
        key_index_1 = key_index + 1

        # Set the private key template, it will be expanded later in 'swu' recipes once keys are generated.
        if (d.getVar("TRUSTFENCE_SIGN_MODE", "") == "AHAB"):
            d.setVar("SWUPDATE_PRIVATE_KEY_TEMPLATE", keys_path + "/keys/SRK" + str(key_index_1) + "*key.pem")
            d.setVar("CONFIG_SIGN_MODE", "AHAB")
        else:
            d.setVar("SWUPDATE_PRIVATE_KEY_TEMPLATE", keys_path + "/keys/IMG" + str(key_index_1) + "*key.pem")
            d.setVar("CONFIG_SIGN_MODE", "HAB")

        # Set the key password.
        d.setVar("SWUPDATE_PASSWORD_FILE", keys_path + "/keys/key_pass.txt")

    # Enable partition encryption if rootfs encryption is enabled
    if (d.getVar("TRUSTFENCE_ENCRYPT_ROOTFS") == "1"):
        d.setVar("TRUSTFENCE_ENCRYPT_PARTITIONS", "1");

    # Enable the trustfence initramfs if and only if partition encryption is enabled
    # and not using a read-only rootfs
    if (d.getVar("TRUSTFENCE_ENCRYPT_PARTITIONS") == "1" and \
        d.getVar("STORAGE_MEDIA") == "mmc" and \
        d.getVar("TRUSTFENCE_READ_ONLY_ROOTFS") == "0"):
        d.setVar("TRUSTFENCE_INITRAMFS_IMAGE", "dey-image-trustfence-initramfs");
    else:
        d.setVar("TRUSTFENCE_INITRAMFS_IMAGE", "");
}

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

TRUSTFENCE_ENABLED = "1"

# Default secure console configuration
TRUSTFENCE_CONSOLE_DISABLE ?= "0"

# Uncomment to enable the console with the specified passphrase
#TRUSTFENCE_CONSOLE_PASSPHRASE_ENABLE = "my_secure_passphrase"

# Alternatively, uncommment to enable the console with the specified GPIO
#TRUSTFENCE_CONSOLE_GPIO_ENABLE = "4"

# Default secure boot configuration
TRUSTFENCE_SIGN ?= "1"
TRUSTFENCE_SIGN_KEYS_PATH ?= "default"
TRUSTFENCE_DEK_PATH ?= "${TF_DEK_PATH}"
TRUSTFENCE_ENCRYPT_ENVIRONMENT ?= "1"
TRUSTFENCE_SRK_REVOKE_MASK ?= "0x0"
TRUSTFENCE_KEY_INDEX ?= "0"
TRUSTFENCE_SIGN_ARTIFACTS = "1"
TRUSTFENCE_SIGN_ARTIFACTS:ccmp1 = "0"
TRUSTFENCE_SIGN_FIT_STM:ccmp1 ?= "1"

# Partition encryption configuration
TRUSTFENCE_ENCRYPT_PARTITIONS ?= "1"
TRUSTFENCE_ENCRYPT_PARTITIONS:ccimx9 ?= "0"
TRUSTFENCE_ENCRYPT_ROOTFS ?= "${@bb.utils.contains("IMAGE_FEATURES", "read-only-rootfs", "0", "1", d)}"
TRUSTFENCE_ENCRYPT_ROOTFS:ccimx9 ?= "0"
TRUSTFENCE_FILE_BASED_ENCRYPT ?= "${TF_FILE_BASED_ENCRYPT}"

# Read-only rootfs
TRUSTFENCE_READ_ONLY_ROOTFS ?= "${@bb.utils.contains("IMAGE_FEATURES", "read-only-rootfs", "1", "0", d)}"

#
# NOTHING TO CUSTOMIZE BELOW THIS LINE
#

# Platform specific defaults
TF_DEK_PATH = "default"
TF_DEK_PATH:ccimx9 = "0"
TF_DEK_PATH:ccmp1 = "0"
TF_FILE_BASED_ENCRYPT = "0"
TF_FILE_BASED_ENCRYPT:ccimx9 = "1"
TF_FILE_BASED_ENCRYPT:ccmp1 = "1"

# NXP-based sign a FIT-format boot artifact
TRUSTFENCE_SIGN_FIT_NXP = "0"
TRUSTFENCE_SIGN_FIT_NXP:ccimx9 = "${TRUSTFENCE_SIGN_ARTIFACTS}"

IMAGE_FEATURES += "dey-trustfence"

# ---------------------------------
#  Usage of FIT Image signed
# ---------------------------------

# key to sign FIT config nodes
TRUSTFENCE_FIT_CFG_SIGN_KEYNAME ?= "fitcfg"
# key to sign FIT image nodes
TRUSTFENCE_FIT_IMG_SIGN_KEYNAME ?= "fitimg"

# Function to generate a PKI tree (with lock dir protection)
GENPKI_LOCK_DIR = "${TRUSTFENCE_SIGN_KEYS_PATH}/.genpki.lock"
gen_pki_tree() {
	if mkdir -p ${GENPKI_LOCK_DIR}; then
		if [ "${DEY_SOC_VENDOR}" = "NXP" ]; then
			trustfence-gen-pki.sh ${TRUSTFENCE_SIGN_KEYS_PATH}
		elif [ "${DEY_SOC_VENDOR}" = "STM" ]; then
			export CONFIG_SIGN_KEYS_PATH="${TRUSTFENCE_SIGN_KEYS_PATH}"
			trustfence-gen-pki.sh -p ${DIGI_SOM}
		fi
		rm -rf ${GENPKI_LOCK_DIR}
	else
		bbfatal "Could not get lock to generate PKI tree"
	fi
}

# Function that generates a PKI tree if there isn't one
check_gen_pki_tree() {
	if [ "${DEY_SOC_VENDOR}" = "NXP" ]; then
		SRK_KEYS="$(echo ${TRUSTFENCE_SIGN_KEYS_PATH}/crts/SRK*crt.pem | sed s/\ /\,/g)"
		n_commas="$(echo ${SRK_KEYS} | grep -o "," | wc -l)"
		if [ "${n_commas}" -eq 0 ]; then
			gen_pki_tree
		elif [ "${n_commas}" -ne 3 ]; then
			bbfatal "Inconsistent PKI tree"
		fi
	elif [ "${DEY_SOC_VENDOR}" = "STM" ]; then
		# The script that generates the PKI tree already checks if
		# there isn't one, so there's nothing to do here but calling it.
		gen_pki_tree
	fi
}

copy_public_key() {
	if [ "${DEY_SOC_VENDOR}" = "NXP" ]; then
		KEY_INDEX="$(expr ${TRUSTFENCE_KEY_INDEX} + 1)"
		PUBLIC_KEY="${TRUSTFENCE_SIGN_KEYS_PATH}/crts/key${KEY_INDEX}.pub"
		# The new hab/ahab_pki_tree.sh script extracts the public keys after the PKI
		# generation and leaves them in the crts/ folder. However, the PKI tree may
		# already exist, the PKI generation script not called, and then the public
		# keys may not be available. This is a fall-back to generate at least the
		# selected public key.
		if [ ! -f "${PUBLIC_KEY}" ]; then
			if [ "${TRUSTFENCE_SIGN_MODE}" = "HAB" ]; then
				CERT_IMG="$(echo ${TRUSTFENCE_SIGN_KEYS_PATH}/crts/IMG${KEY_INDEX}*crt.pem)"
			elif [ "${TRUSTFENCE_SIGN_MODE}" = "AHAB" ]; then
				CERT_IMG="$(echo ${TRUSTFENCE_SIGN_KEYS_PATH}/crts/SRK${KEY_INDEX}*crt.pem)"
			else
				bberror "Unknown TRUSTFENCE_SIGN_MODE value"
				exit 1
			fi
			# Extract the public key from the certificate.
			openssl x509 -pubkey -noout -in "${CERT_IMG}" > "${PUBLIC_KEY}"
		fi
	elif [ "${DEY_SOC_VENDOR}" = "STM" ]; then
		if [ "${DIGI_SOM}" = "ccmp15" ]; then
			PUBLIC_KEY="${TRUSTFENCE_SIGN_KEYS_PATH}/keys/publicKey.pem"
		elif [ "${DIGI_SOM}" = "ccmp13" ]; then
			PUBLIC_KEY="${TRUSTFENCE_SIGN_KEYS_PATH}/keys/publicKey0${TRUSTFENCE_KEY_INDEX}.pem"
		else
			bberror "Unknown DIGI_SOM"
			exit 1
		fi
	else
		echo "ERROR: Cannot determine the public key"
		exit 1
	fi
	# Copy the public key to the rootfs
	install -d ${IMAGE_ROOTFS}${sysconfdir}/ssl/certs
	cp -f "${PUBLIC_KEY}" "${IMAGE_ROOTFS}${sysconfdir}/ssl/certs/key.pub"
}
ROOTFS_POSTPROCESS_COMMAND:append = " copy_public_key;"

python () {
    import binascii
    import hashlib
    import os

    # Secure console configuration
    if (d.getVar("TRUSTFENCE_CONSOLE_DISABLE") == "1"):
        d.appendVar("UBOOT_TF_CONF", "CONFIG_CONSOLE_DISABLE=y ")
        if d.getVar("TRUSTFENCE_CONSOLE_PASSPHRASE_ENABLE"):
            passphrase_hash = hashlib.sha256(d.getVar("TRUSTFENCE_CONSOLE_PASSPHRASE_ENABLE").encode()).hexdigest()
            if (d.getVar("DEY_SOC_VENDOR") == "NXP"):
                d.appendVar("UBOOT_TF_CONF", 'CONFIG_CONSOLE_ENABLE_PASSPHRASE=y CONFIG_CONSOLE_ENABLE_PASSPHRASE_KEY="%s" ' % passphrase_hash)
            elif (d.getVar("DEY_SOC_VENDOR") == "STM"):
                d.appendVar("UBOOT_TF_CONF", 'CONFIG_AUTOBOOT_KEYED=y CONFIG_AUTOBOOT_ENCRYPTION=y CONFIG_AUTOBOOT_STOP_STR_ENABLE=y CONFIG_AUTOBOOT_STOP_STR_SHA256="%s" ' % passphrase_hash)
        elif d.getVar("TRUSTFENCE_CONSOLE_GPIO_ENABLE"):
            if (d.getVar("DEY_SOC_VENDOR") == "NXP"):
                d.appendVar("UBOOT_TF_CONF", "CONFIG_CONSOLE_ENABLE_GPIO=y CONFIG_CONSOLE_ENABLE_GPIO_NR=%s " % d.getVar("TRUSTFENCE_CONSOLE_GPIO_ENABLE"))
                if d.getVar("TRUSTFENCE_CONSOLE_GPIO_ENABLE_NAME"):
                    d.appendVar("UBOOT_TF_CONF", 'CONFIG_CONSOLE_ENABLE_GPIO_NAME="%s" ' % d.getVar("TRUSTFENCE_CONSOLE_GPIO_ENABLE_NAME"))
                    if d.getVar("TRUSTFENCE_CONSOLE_GPIO_ENABLE_ACTIVE_LOW"):
                        d.appendVar("UBOOT_TF_CONF", "CONFIG_CONSOLE_ENABLE_GPIO_ACTIVE_LOW=y ")
                    else:
                        d.appendVar("UBOOT_TF_CONF", '"# CONFIG_CONSOLE_ENABLE_GPIO_ACTIVE_LOW is not set" ')
            elif (d.getVar("DEY_SOC_VENDOR") == "STM"):
                d.appendVar("UBOOT_TF_CONF", 'CONFIG_CONSOLE_ENABLE_GPIO=y CONFIG_CONSOLE_ENABLE_GPIO_NAME="%s" ' % d.getVar("TRUSTFENCE_CONSOLE_GPIO_ENABLE_NAME"))

    # Secure boot configuration
    if (d.getVar("TRUSTFENCE_SIGN_KEYS_PATH") == "default"):
        d.setVar("TRUSTFENCE_SIGN_KEYS_PATH", d.getVar("TOPDIR") + "/trustfence");

    if (d.getVar("DEY_SOC_VENDOR") == "NXP"):
        if (d.getVar("TRUSTFENCE_DEK_PATH") == "default"):
            d.setVar("TRUSTFENCE_DEK_PATH", d.getVar("TRUSTFENCE_SIGN_KEYS_PATH") + "/dek.bin");
    elif (d.getVar("DEY_SOC_VENDOR") == "STM"):
        # Enable authentication capabilities on TF-A independently
        # of whether the images are going to be signed by DEY or externally
        d.setVar("TF_A_SIGN_ENABLE", "1")
        if (d.getVar("TRUSTFENCE_SIGN") == "0"):
            d.setVar("FIP_SIGN_ENABLE", "0")

    if (d.getVar("TRUSTFENCE_SIGN") == "1"):
        # Set STM-specific variables for signing images
        if (d.getVar("DEY_SOC_VENDOR") == "STM"):
            d.setVar("FIP_SIGN_ENABLE", "1")
            d.setVar("FIP_SIGN_KEY_EXTERNAL", "1")
            if (d.getVar("DIGI_SOM") == "ccmp15" ):
                d.setVar("FIP_SIGN_KEY", d.getVar("TRUSTFENCE_SIGN_KEYS_PATH") + "/keys/privateKey.pem");
                d.setVar("TRUSTFENCE_PASSWORD_FILE", d.getVar("TRUSTFENCE_SIGN_KEYS_PATH") + "/keys/key_pass.txt")
            elif (d.getVar("DIGI_SOM") == "ccmp13" ):
                d.setVar("FIP_SIGN_KEY", d.getVar("TRUSTFENCE_SIGN_KEYS_PATH") + "/keys/privateKey0%s.pem" % d.getVar("TRUSTFENCE_KEY_INDEX"));
                d.setVar("TRUSTFENCE_PASSWORD_FILE", d.getVar("TRUSTFENCE_SIGN_KEYS_PATH") + "/keys/key_pass0%s.txt" % d.getVar("TRUSTFENCE_KEY_INDEX"))

        d.appendVar("UBOOT_TF_CONF", "CONFIG_SIGN_IMAGE=y ")
        if (d.getVar("TRUSTFENCE_SIGN_ARTIFACTS") == "1"):
            d.appendVar("UBOOT_TF_CONF", "CONFIG_AUTH_ARTIFACTS=y ")
            if (d.getVar("TRUSTFENCE_SIGN_FIT_NXP") == "1"):
                d.appendVar("UBOOT_TF_CONF", '"# CONFIG_CMD_BOOTI is not set" ')
                d.appendVar("UBOOT_TF_CONF", '"# CONFIG_LEGACY_IMAGE_FORMAT is not set" ')
        if (d.getVar("TRUSTFENCE_READ_ONLY_ROOTFS") == "1"):
            d.appendVar("UBOOT_TF_CONF", "CONFIG_AUTHENTICATE_SQUASHFS_ROOTFS=y ")
        if d.getVar("TRUSTFENCE_SIGN_KEYS_PATH"):
            d.appendVar("UBOOT_TF_CONF", 'CONFIG_SIGN_KEYS_PATH="%s" ' % d.getVar("TRUSTFENCE_SIGN_KEYS_PATH"))
        if (d.getVar("TRUSTFENCE_UNLOCK_KEY_REVOCATION") == "1"):
            d.appendVar("UBOOT_TF_CONF", "CONFIG_UNLOCK_SRK_REVOKE=y ")
        if d.getVar("TRUSTFENCE_KEY_INDEX"):
            d.appendVar("UBOOT_TF_CONF", "CONFIG_KEY_INDEX=%s " % d.getVar("TRUSTFENCE_KEY_INDEX"))
        if (d.getVar("DEY_SOC_VENDOR") == "NXP"):
            if (d.getVar("TRUSTFENCE_DEK_PATH") not in [None, "0"]):
                d.appendVar("UBOOT_TF_CONF", 'CONFIG_DEK_PATH="%s" ' % d.getVar("TRUSTFENCE_DEK_PATH"))
            if d.getVar("TRUSTFENCE_SIGN_MODE"):
                d.appendVar("UBOOT_TF_CONF", 'CONFIG_SIGN_MODE="%s" ' % d.getVar("TRUSTFENCE_SIGN_MODE"))


        if (d.getVar("TRUSTFENCE_SIGN_FIT_STM") == "1"):
            # FIT-related variables
            # Create keys if not defined
            d.setVar("FIT_GENERATE_KEYS", "1")
            # Sign individual images (prevents running unsigned images in FIT)
            d.setVar("FIT_SIGN_INDIVIDUAL", "1")
            # Set variables required by poky to sign FIT image
            d.setVar("UBOOT_SIGN_KEYNAME", d.getVar("TRUSTFENCE_FIT_CFG_SIGN_KEYNAME"))
            d.setVar("UBOOT_SIGN_IMG_KEYNAME", d.getVar("TRUSTFENCE_FIT_IMG_SIGN_KEYNAME"))
            d.setVar("UBOOT_MKIMAGE_DTCOPTS", "-I dts -O dtb -p 2000")
            # Enable FIT signing support
            d.setVar("UBOOT_SIGN_ENABLE", d.getVar("TRUSTFENCE_SIGN"))
            # Set path to FIT signing keys
            d.setVar("UBOOT_SIGN_KEYDIR", "%s/fit" % d.getVar("TRUSTFENCE_SIGN_KEYS_PATH"))

    else:
        # Disable signing artifacts if TRUSTFENCE_SIGN != 1
        d.setVar("TRUSTFENCE_SIGN_ARTIFACTS", "0")

    if (d.getVar("TRUSTFENCE_ENCRYPT_ENVIRONMENT") == "1"):
        if (d.getVar("DEY_SOC_VENDOR") == "NXP"):
            d.appendVar("UBOOT_TF_CONF", "CONFIG_ENV_AES=y CONFIG_ENV_AES_CAAM_KEY=y CONFIG_ENV_ENCRYPT=y ")
        elif (d.getVar("DEY_SOC_VENDOR") == "STM"):
            d.appendVar("UBOOT_TF_CONF", "CONFIG_ENV_AES_CCMP1=y ")

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
        if (d.getVar("DEY_SOC_VENDOR") == "NXP"):
            if (d.getVar("TRUSTFENCE_SIGN_MODE", "") == "AHAB"):
                d.setVar("SWUPDATE_PRIVATE_KEY_TEMPLATE", keys_path + "/keys/SRK" + str(key_index_1) + "*key.pem")
                d.setVar("CONFIG_SIGN_MODE", "AHAB")
            else:
                d.setVar("SWUPDATE_PRIVATE_KEY_TEMPLATE", keys_path + "/keys/IMG" + str(key_index_1) + "*key.pem")
                d.setVar("CONFIG_SIGN_MODE", "HAB")
            # Set the key password.
            d.setVar("SWUPDATE_PASSWORD_FILE", keys_path + "/keys/key_pass.txt")
        elif (d.getVar("DEY_SOC_VENDOR") == "STM"):
            d.setVar("SWUPDATE_PRIVATE_KEY_TEMPLATE", d.getVar("FIP_SIGN_KEY"))
            # Set the key password.
            if (d.getVar("DIGI_SOM") == "ccmp15"):
                d.setVar("SWUPDATE_PASSWORD_FILE", keys_path + "/keys/key_pass.txt")
            elif (d.getVar("DIGI_SOM") == "ccmp13"):
                d.setVar("SWUPDATE_PASSWORD_FILE", keys_path + "/keys/key_pass0" + str(key_index) + ".txt")

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

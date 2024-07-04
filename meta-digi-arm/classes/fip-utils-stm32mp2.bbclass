inherit sign-stm32mp2

DEPENDS += "tf-a-tools-native util-linux-native"

# Configure new package to provide fiptool wrapper for SDK usage
PACKAGES =+ "${FIPTOOL_WRAPPER}"

BBCLASSEXTEND:append = " nativesdk"

RRECOMMENDS:${FIPTOOL_WRAPPER}:append:class-nativesdk = " nativesdk-tf-a-tools"

# Define default TF-A FIP namings
FIP_BASENAME ?= "fip"
FIP_SUFFIX   ?= "bin"

# Set default TF-A FIP config
FIP_CONFIG ?= ""

# Default FIP config:
#   There are two options implemented to select two different firmware and each
#   FIP_CONFIG should configure one: 'tfa' or 'optee'
FIP_CONFIG_FW_TFA = "tfa"
FIP_CONFIG_FW_TEE = "optee"

# Init BL31 config
FIP_BL31_ENABLE ?= ""

# Set CERTTOOL binary name to use
CERTTOOL ?= "cert_create"
# Set ENCTOOL binary name to use
ENCTOOL ?= "encrypt_fw"
# Set FIPTOOL binary name to use
FIPTOOL ?= "fiptool"
# Set STM32MP fiptool wrapper
FIPTOOL_WRAPPER ?= "fiptool-stm32mp"

# Default FIP file names and suffixes
FIP_BL31        ?= "tf-a-bl31"
FIP_BL31_SUFFIX ?= "bin"
FIP_BL31_DTB        ?= "bl31"
FIP_BL31_DTB_SUFFIX ?= "dtb"
FIP_TFA        ?= "tf-a-bl32"
FIP_TFA_SUFFIX ?= "bin"
FIP_TFA_DTB        ?= "bl32"
FIP_TFA_DTB_SUFFIX ?= "dtb"
FIP_FW_CONFIG        ?= "fw-config"
FIP_FW_CONFIG_SUFFIX ?= "dtb"
FIP_FW_DDR        ?= "ddr_pmu"
FIP_FW_DDR_SUFFIX ?= "bin"
FIP_OPTEE_HEADER   ?= "tee-header_v2"
FIP_OPTEE_PAGER    ?= "tee-pager_v2"
FIP_OPTEE_PAGEABLE ?= "tee-pageable_v2"
FIP_OPTEE_SUFFIX   ?= "bin"
FIP_UBOOT        ?= "u-boot-nodtb"
FIP_UBOOT_SUFFIX ?= "bin"
FIP_UBOOT_DTB        ?= "u-boot"
FIP_UBOOT_DTB_SUFFIX ?= "dtb"

# Configure default folder path for binaries to package
FIP_DEPLOYDIR_FIP    ?= "${DEPLOYDIR}/fip"
FIP_DEPLOYDIR_BL31   ?= "${DEPLOYDIR}/arm-trusted-firmware/bl31"
FIP_DEPLOYDIR_TFA    ?= "${DEPLOYDIR}/arm-trusted-firmware/bl32"
FIP_DEPLOYDIR_FWCONF ?= "${DEPLOYDIR}/arm-trusted-firmware/fwconfig"
FIP_DEPLOYDIR_FWDDR  ?= "${DEPLOYDIR}/arm-trusted-firmware/ddr"
FIP_DEPLOYDIR_OPTEE  ?= "${DEPLOY_DIR}/images/${MACHINE}/optee"
FIP_DEPLOYDIR_UBOOT  ?= "${DEPLOY_DIR}/images/${MACHINE}/u-boot"

# Set default configuration to allow FIP signing
FIP_ENCRYPT_SUFFIX ??= "${@bb.utils.contains('ENCRYPT_ENABLE', '1', '${ENCRYPT_SUFFIX}', '', d)}"
FIP_ENCRYPT_NONCE ??= "1234567890abcdef12345678"
FIP_SIGN_SUFFIX ??= "${@bb.utils.contains('SIGN_ENABLE', '1', '${SIGN_SUFFIX}', '', d)}"

# Define FIP dependency build
FIP_DEPENDS += "virtual/bootloader"
FIP_DEPENDS += "${@bb.utils.contains('MACHINE_FEATURES', 'optee', 'virtual/optee-os', '', d)}"
FIP_DEPENDS:class-nativesdk = ""

# -----------------------------------------------
# Handle FIP config and set internal vars
#   FIP_BL32_CONF
python () {
    import re

    # Make sure that deploy class is configured
    if not bb.data.inherits_class('deploy', d):
         bb.fatal("The st-fip-utils class needs the deploy class to be configured on recipe side.")

    # Manage FIP binary dependencies
    fip_depends = (d.getVar('FIP_DEPENDS') or "").split()
    if len(fip_depends) > 0:
        for depend in fip_depends:
            d.appendVarFlag('do_deploy', 'depends', ' %s:do_deploy' % depend)

    # Manage FIP config settings
    fipconfigflags = d.getVarFlags('FIP_CONFIG')
    if fipconfigflags is not None:
        # The "doc" varflag is special, we don't want to see it here
        fipconfigflags.pop('doc', None)
    fipconfig = (d.getVar('FIP_CONFIG') or "").split()
    if not fipconfig:
        raise bb.parse.SkipRecipe("FIP_CONFIG must be set in the %s machine configuration." % d.getVar("MACHINE"))
    if (d.getVar('FIP_BL32_CONF') or "").split():
        raise bb.parse.SkipRecipe("You cannot use FIP_BL32_CONF as it is internal to FIP_CONFIG var expansion.")
    if (d.getVar('FIP_DEVICETREE') or "").split():
        raise bb.parse.SkipRecipe("You cannot use FIP_DEVICETREE as it is internal to FIP_CONFIG var expansion.")
    if len(fipconfig) > 0:
        # Init internal fip firmware config
        fip_config_fw_tfa = d.getVar('FIP_CONFIG_FW_TFA') or ""
        fip_config_fw_tee = d.getVar('FIP_CONFIG_FW_TEE') or ""
        for config in fipconfig:
            for f, v in fipconfigflags.items():
                if config == f:
                    # Make sure to get var flag properly expanded
                    v = d.getVarFlag('FIP_CONFIG', config)
                    if not v.strip():
                        bb.fatal('[FIP_CONFIG] Missing configuration for %s config' % config)
                    items = v.split(',')
                    if items[0] and len(items) > 2:
                        raise bb.parse.SkipRecipe('Only <BL32_CONF> and <DT_CONFIG> can be specified!')
                    # Set internal vars
                    if items[0] == fip_config_fw_tfa or items[0] == fip_config_fw_tee:
                        bb.debug(1, "Appending '%s' to FIP_BL32_CONF" % items[0])
                        d.appendVar('FIP_BL32_CONF', items[0] + ',')
                    else:
                        bb.fatal('[FIP_CONFIG] Wrong configuration for %s config: %s should be one of %s or %s' % (config,items[0],fip_config_fw_tfa,fip_config_fw_tee))
                    bb.debug(1, "Appending '%s' to FIP_DEVICETREE" % items[1])
                    d.appendVar('FIP_DEVICETREE', items[1] + ',')
                    break
}

# Deploy the fip binary for current target
do_deploy:append:class-target() {
    install -d ${DEPLOYDIR}
    install -d ${FIP_DEPLOYDIR_FIP}

    unset i
    for config in ${FIP_CONFIG}; do
        i=$(expr $i + 1)
        bl32_conf=$(echo ${FIP_BL32_CONF} | cut -d',' -f${i})
        dt_config=$(echo ${FIP_DEVICETREE} | cut -d',' -f${i})
        for dt in ${dt_config}; do
            # Init soc suffix
            soc_suffix=""
            if [ -n "${STM32MP_SOC_NAME}" ]; then
                for soc in ${STM32MP_SOC_NAME}; do
                    [ "$(echo ${dt} | grep -c ${soc})" -eq 1 ] && soc_suffix="-${soc}"
                done
            fi
            # Init FIP fw-config settings
            [ -f "${FIP_DEPLOYDIR_FWCONF}/${dt}-${FIP_FW_CONFIG}-${config}.${FIP_FW_CONFIG_SUFFIX}" ] || bbfatal "Missing ${dt}-${FIP_FW_CONFIG}-${config}.${FIP_FW_CONFIG_SUFFIX} file in folder: ${FIP_DEPLOYDIR_FWCONF}"
            FIP_FWCONFIG="--fw-config ${FIP_DEPLOYDIR_FWCONF}/${dt}-${FIP_FW_CONFIG}-${config}.${FIP_FW_CONFIG_SUFFIX}"
            # Init FIP hw-config settings
            [ -f "${FIP_DEPLOYDIR_UBOOT}/${FIP_UBOOT_DTB}-${dt}.${FIP_UBOOT_DTB_SUFFIX}" ] || bbfatal "Missing ${FIP_UBOOT_DTB}-${dt}.${FIP_UBOOT_DTB_SUFFIX} file in folder: ${FIP_DEPLOYDIR_UBOOT}"
            FIP_HWCONFIG="--hw-config ${FIP_DEPLOYDIR_UBOOT}/${FIP_UBOOT_DTB}-${dt}.${FIP_UBOOT_DTB_SUFFIX}"
            # Init FIP nt-fw config
            [ -f "${FIP_DEPLOYDIR_UBOOT}/${FIP_UBOOT}${soc_suffix}.${FIP_UBOOT_SUFFIX}" ] || bbfatal "Missing ${FIP_UBOOT}${soc_suffix}.${FIP_UBOOT_SUFFIX} file in folder: ${FIP_DEPLOYDIR_UBOOT}"
            FIP_NTFW="--nt-fw ${FIP_DEPLOYDIR_UBOOT}/${FIP_UBOOT}${soc_suffix}.${FIP_UBOOT_SUFFIX}"
            # Init FIP bl31 settings
            if [ "${FIP_BL31_ENABLE}" = "1" ]; then
                # Check for files
                [ -f "${FIP_DEPLOYDIR_BL31}/${FIP_BL31}${soc_suffix}.${FIP_BL31_SUFFIX}" ] || bbfatal "Missing ${FIP_BL31}${soc_suffix}.${FIP_BL31_SUFFIX} file in folder: ${FIP_DEPLOYDIR_BL31}"
                [ -f "${FIP_DEPLOYDIR_BL31}/${dt}-${FIP_BL31_DTB}.${FIP_BL31_DTB_SUFFIX}" ] || bbfatal "Missing ${dt}-${FIP_BL31_DTB}.${FIP_BL31_DTB_SUFFIX} file in folder: ${FIP_DEPLOYDIR_BL31}"
                # Set CERT_BL31CONF
                CERT_BL31CONF=" \
                        --soc-fw ${FIP_DEPLOYDIR_BL31}/${FIP_BL31}${soc_suffix}.${FIP_BL31_SUFFIX} \
                        --soc-fw-config ${FIP_DEPLOYDIR_BL31}/${dt}-${FIP_BL31_DTB}.${FIP_BL31_DTB_SUFFIX} \
                        "
                if [ "${ENCRYPT_ENABLE}" = "1" ]; then
                    encrypt_key="${ENCRYPT_FIP_KEY_PATH_LIST}"
                    if [ -n "${STM32MP_ENCRYPT_SOC_NAME}" ]; then
                        unset k
                        for soc in ${STM32MP_ENCRYPT_SOC_NAME}; do
                            k=$(expr $k + 1)
                            [ "$(echo ${dt} | grep -c ${soc})" -eq 1 ] && encrypt_key=$(echo ${ENCRYPT_FIP_KEY_PATH_LIST} | cut -d',' -f${k})
                        done
                    fi
                    encrypt_key="$(hexdump -e '/1 "%02x"' ${encrypt_key})"

                    # encrypt bl31 binary
                    bbnote "${ENCTOOL} --key ${encrypt_key} --nonce ${FIP_ENCRYPT_NONCE} --fw-enc-status 0 \
                            --in \"${FIP_DEPLOYDIR_BL31}/${FIP_BL31}${soc_suffix}.${FIP_BL31_SUFFIX}\" \
                            --out \"${FIP_DEPLOYDIR_BL31}/${FIP_BL31}${soc_suffix}${FIP_ENCRYPT_SUFFIX}.${FIP_BL31_SUFFIX}\" "
                    ${ENCTOOL} --key ${encrypt_key} --nonce ${FIP_ENCRYPT_NONCE} --fw-enc-status 0 \
                            --in "${FIP_DEPLOYDIR_BL31}/${FIP_BL31}${soc_suffix}.${FIP_BL31_SUFFIX}" \
                            --out "${FIP_DEPLOYDIR_BL31}/${FIP_BL31}${soc_suffix}${FIP_ENCRYPT_SUFFIX}.${FIP_BL31_SUFFIX}"
                    # encrypt bl31 devicetree
                    bbnote "${ENCTOOL} --key ${encrypt_key} --nonce ${FIP_ENCRYPT_NONCE} --fw-enc-status 0 \
                            --in \"${FIP_DEPLOYDIR_BL31}/${dt}-${FIP_BL31_DTB}.${FIP_BL31_DTB_SUFFIX}\" \
                            --out \"${FIP_DEPLOYDIR_BL31}/${dt}-${FIP_BL31_DTB}${FIP_ENCRYPT_SUFFIX}.${FIP_BL31_DTB_SUFFIX} \" "
                    ${ENCTOOL} --key ${encrypt_key} --nonce ${FIP_ENCRYPT_NONCE} --fw-enc-status 0 \
                            --in "${FIP_DEPLOYDIR_BL31}/${dt}-${FIP_BL31_DTB}.${FIP_BL31_DTB_SUFFIX}" \
                            --out "${FIP_DEPLOYDIR_BL31}/${dt}-${FIP_BL31_DTB}${FIP_ENCRYPT_SUFFIX}.${FIP_BL31_DTB_SUFFIX}"
                fi
                # Set FIP_BL31CONF
                FIP_BL31CONF="\
                    --soc-fw ${FIP_DEPLOYDIR_BL31}/${FIP_BL31}${soc_suffix}${FIP_ENCRYPT_SUFFIX}.${FIP_BL31_SUFFIX} \
                    --soc-fw-config ${FIP_DEPLOYDIR_BL31}/${dt}-${FIP_BL31_DTB}${FIP_ENCRYPT_SUFFIX}.${FIP_BL31_DTB_SUFFIX} \
                    "
              else
                CERT_BL31CONF=""
                FIP_BL31CONF=""
            fi
            # Init FIP extra conf settings
            if [ "${bl32_conf}" = "${FIP_CONFIG_FW_TFA}" ]; then
                # Check for files
                [ -f "${FIP_DEPLOYDIR_TFA}/${FIP_TFA}${soc_suffix}.${FIP_TFA_SUFFIX}" ] || bbfatal "Missing ${FIP_TFA}${soc_suffix}.${FIP_TFA_SUFFIX} file in folder: ${FIP_DEPLOYDIR_TFA}"
                [ -f "${FIP_DEPLOYDIR_TFA}/${dt}-${FIP_TFA_DTB}.${FIP_TFA_DTB_SUFFIX}" ] || bbfatal "Missing ${dt}-${FIP_TFA_DTB}.${FIP_TFA_DTB_SUFFIX} file in folder: ${FIP_DEPLOYDIR_TFA}"
                # Set FIP_EXTRACONF
                FIP_EXTRACONF="\
                    --tos-fw ${FIP_DEPLOYDIR_TFA}/${FIP_TFA}${soc_suffix}.${FIP_TFA_SUFFIX} \
                    --tos-fw-config ${FIP_DEPLOYDIR_TFA}/${dt}-${FIP_TFA_DTB}.${FIP_TFA_DTB_SUFFIX} \
                    "
            elif [ "${bl32_conf}" = "${FIP_CONFIG_FW_TEE}" ]; then
                # Check for files
                [ -f "${FIP_DEPLOYDIR_OPTEE}/${FIP_OPTEE_HEADER}-${dt}.${FIP_OPTEE_SUFFIX}" ] || bbfatal "Missing ${FIP_OPTEE_HEADER}-${dt}.${FIP_OPTEE_SUFFIX} file in folder: ${FIP_DEPLOYDIR_OPTEE}"
                [ -f "${FIP_DEPLOYDIR_OPTEE}/${FIP_OPTEE_PAGER}-${dt}.${FIP_OPTEE_SUFFIX}" ] || bbfatal "Missing ${FIP_OPTEE_PAGER}-${dt}.${FIP_OPTEE_SUFFIX} file in folder: ${FIP_DEPLOYDIR_OPTEE}"
                [ -f "${FIP_DEPLOYDIR_OPTEE}/${FIP_OPTEE_PAGEABLE}-${dt}.${FIP_OPTEE_SUFFIX}" ] || bbfatal "Missing ${FIP_OPTEE_PAGEABLE}-${dt}.${FIP_OPTEE_SUFFIX} file in folder: ${FIP_DEPLOYDIR_OPTEE}"
                # Set CERT_EXTRACONF
                CERT_EXTRACONF="\
                    --tos-fw ${FIP_DEPLOYDIR_OPTEE}/${FIP_OPTEE_HEADER}-${dt}.${FIP_OPTEE_SUFFIX} \
                    --tos-fw-extra1 ${FIP_DEPLOYDIR_OPTEE}/${FIP_OPTEE_PAGER}-${dt}.${FIP_OPTEE_SUFFIX} \
                    --tos-fw-extra2 ${FIP_DEPLOYDIR_OPTEE}/${FIP_OPTEE_PAGEABLE}-${dt}.${FIP_OPTEE_SUFFIX} \
                    "
                if [ "${ENCRYPT_ENABLE}" = "1" ]; then
                    encrypt_key="${ENCRYPT_FIP_KEY_PATH_LIST}"
                    if [ -n "${STM32MP_ENCRYPT_SOC_NAME}" ]; then
                        unset k
                        for soc in ${STM32MP_ENCRYPT_SOC_NAME}; do
                            k=$(expr $k + 1)
                            [ "$(echo ${dt} | grep -c ${soc})" -eq 1 ] && encrypt_key=$(echo ${ENCRYPT_FIP_KEY_PATH_LIST} | cut -d',' -f${k})
                        done
                    fi
                    encrypt_key="$(hexdump -e '/1 "%02x"' ${encrypt_key})"
                    # encrypt optee header
                    bbnote "${ENCTOOL} --key ${encrypt_key} --nonce ${FIP_ENCRYPT_NONCE} --fw-enc-status 0 \
                        --in \"${FIP_DEPLOYDIR_OPTEE}/${FIP_OPTEE_HEADER}-${dt}.${FIP_OPTEE_SUFFIX}\" \
                        --out \"${FIP_DEPLOYDIR_OPTEE}/${FIP_OPTEE_HEADER}-${dt}${FIP_ENCRYPT_SUFFIX}.${FIP_OPTEE_SUFFIX}\" "
                    ${ENCTOOL} --key ${encrypt_key} --nonce ${FIP_ENCRYPT_NONCE} --fw-enc-status 0 \
                        --in "${FIP_DEPLOYDIR_OPTEE}/${FIP_OPTEE_HEADER}-${dt}.${FIP_OPTEE_SUFFIX}" \
                        --out "${FIP_DEPLOYDIR_OPTEE}/${FIP_OPTEE_HEADER}-${dt}${FIP_ENCRYPT_SUFFIX}.${FIP_OPTEE_SUFFIX}"
                    # encrypt optee pager
                    bbnote "${ENCTOOL} --key ${encrypt_key} --nonce ${FIP_ENCRYPT_NONCE} --fw-enc-status 0 \
                        --in \"${FIP_DEPLOYDIR_OPTEE}/${FIP_OPTEE_PAGER}-${dt}.${FIP_OPTEE_SUFFIX}\" \
                        --out \"${FIP_DEPLOYDIR_OPTEE}/${FIP_OPTEE_PAGER}-${dt}${FIP_ENCRYPT_SUFFIX}.${FIP_OPTEE_SUFFIX}\" "
                    ${ENCTOOL} --key ${encrypt_key} --nonce ${FIP_ENCRYPT_NONCE} --fw-enc-status 0 \
                        --in "${FIP_DEPLOYDIR_OPTEE}/${FIP_OPTEE_PAGER}-${dt}.${FIP_OPTEE_SUFFIX}" \
                        --out "${FIP_DEPLOYDIR_OPTEE}/${FIP_OPTEE_PAGER}-${dt}${FIP_ENCRYPT_SUFFIX}.${FIP_OPTEE_SUFFIX}"
                    # encrypt optee pageable
                    bbnote "${ENCTOOL} --key ${encrypt_key} --nonce ${FIP_ENCRYPT_NONCE} --fw-enc-status 0 \
                        --in \"${FIP_DEPLOYDIR_OPTEE}/${FIP_OPTEE_PAGEABLE}-${dt}.${FIP_OPTEE_SUFFIX}\" \
                        --out \"${FIP_DEPLOYDIR_OPTEE}/${FIP_OPTEE_PAGEABLE}-${dt}${FIP_ENCRYPT_SUFFIX}.${FIP_OPTEE_SUFFIX}\" "
                    ${ENCTOOL} --key ${encrypt_key} --nonce ${FIP_ENCRYPT_NONCE} --fw-enc-status 0 \
                        --in "${FIP_DEPLOYDIR_OPTEE}/${FIP_OPTEE_PAGEABLE}-${dt}.${FIP_OPTEE_SUFFIX}" \
                        --out "${FIP_DEPLOYDIR_OPTEE}/${FIP_OPTEE_PAGEABLE}-${dt}${FIP_ENCRYPT_SUFFIX}.${FIP_OPTEE_SUFFIX}"
                fi
                # Set FIP_EXTRACONF
                FIP_EXTRACONF="\
                    --tos-fw ${FIP_DEPLOYDIR_OPTEE}/${FIP_OPTEE_HEADER}-${dt}${FIP_ENCRYPT_SUFFIX}.${FIP_OPTEE_SUFFIX} \
                    --tos-fw-extra1 ${FIP_DEPLOYDIR_OPTEE}/${FIP_OPTEE_PAGER}-${dt}${FIP_ENCRYPT_SUFFIX}.${FIP_OPTEE_SUFFIX} \
                    --tos-fw-extra2 ${FIP_DEPLOYDIR_OPTEE}/${FIP_OPTEE_PAGEABLE}-${dt}${FIP_ENCRYPT_SUFFIX}.${FIP_OPTEE_SUFFIX} \
                    "
            else
                bbfatal "Wrong configuration '${bl32_conf}' found in FIP_CONFIG for ${config} config."
            fi
            # Init FIP DDR config settings
            if [ -f "${FIP_DEPLOYDIR_FWDDR}/${FIP_FW_DDR}-${dt}.${FIP_FW_DDR_SUFFIX}" ]; then
                FIP_DDRCONF="--ddr-fw ${FIP_DEPLOYDIR_FWDDR}/${FIP_FW_DDR}-${dt}.${FIP_FW_DDR_SUFFIX}"
                CERT_DDRCONF="--ddr-fw ${FIP_DEPLOYDIR_FWDDR}/${FIP_FW_DDR}-${dt}.${FIP_FW_DDR_SUFFIX}"
            else
                FIP_DDRCONF=""
                CERT_DDRCONF=""
            fi
            # Init certificate settings
            if [ "${SIGN_ENABLE}" = "1" ]; then
                sign_key="${SIGN_KEY_PATH_LIST}"
                if [ $(echo ${SIGN_KEY_PASS} | wc -w) -gt 1 ]; then
                    sign_single_key_pass=$(echo ${SIGN_KEY_PASS} | cut -d' ' -f1)
                else
                    sign_single_key_pass="${SIGN_KEY_PASS}"
                fi
                if [ -n "${STM32MP_SOC_NAME}" ]; then
                    unset k
                    for soc in ${STM32MP_SOC_NAME}; do
                        k=$(expr $k + 1)
                        [ "$(echo ${dt} | grep -c ${soc})" -eq 1 ] && sign_key=$(echo ${SIGN_KEY_PATH_LIST} | cut -d',' -f${k})
                    done
                fi
                mkdir -p ${B}/${config}-${dt}
                FIP_CERTCONF="\
                    --tb-fw-cert ${B}/${config}-${dt}/tb_fw.crt \
                    --trusted-key-cert  ${B}/${config}-${dt}/trusted_key.crt \
                    --nt-fw-cert  ${B}/${config}-${dt}/nt_fw_content.crt \
                    --nt-fw-key-cert  ${B}/${config}-${dt}/nt_fw_key.crt \
                    --tos-fw-cert  ${B}/${config}-${dt}/tos_fw_content.crt \
                    --tos-fw-key-cert  ${B}/${config}-${dt}/tos_fw_key.crt \
                    --stm32mp-cfg-cert ${B}/${config}-${dt}/stm32mp_cfg_cert.crt \
                    "
                if [ "${FIP_BL31_ENABLE}" = "1" ]; then
                    FIP_CERTCONF="${FIP_CERTCONF} \
                        --soc-fw-cert  ${B}/${config}-${dt}/soc_fw_content.crt \
                        --soc-fw-key-cert  ${B}/${config}-${dt}/soc_fw_key.crt \
                        "
                fi
                # Need fake bl2 binary to generate certificates
                touch ${WORKDIR}/bl2-fake.bin
                # Generate certificates
                bbnote "${CERTTOOL} -n --tfw-nvctr 0 --ntfw-nvctr 0 --key-alg ecdsa --hash-alg sha256 \
                        --rot-key ${sign_key} \
                        --rot-key-pwd $sign_single_key_pass \
                        ${FIP_FWCONFIG} \
                        ${FIP_HWCONFIG} \
                        ${FIP_NTFW} \
                        ${FIP_CERTCONF} \
                        ${CERT_EXTRACONF} \
                        ${CERT_DDRCONF} \
                        ${CERT_BL31CONF} \
                        --tb-fw ${WORKDIR}/bl2-fake.bin"
                ${CERTTOOL} -n --tfw-nvctr 0 --ntfw-nvctr 0 --key-alg ecdsa --hash-alg sha256 \
                        --rot-key ${sign_key} \
                        --rot-key-pwd $sign_single_key_pass \
                        ${FIP_FWCONFIG} \
                        ${FIP_HWCONFIG} \
                        ${FIP_NTFW} \
                        ${FIP_CERTCONF} \
                        ${CERT_EXTRACONF} \
                        ${CERT_DDRCONF} \
                        ${CERT_BL31CONF} \
                        --tb-fw ${WORKDIR}/bl2-fake.bin
                # Remove fake bl2 binary
                rm -f ${WORKDIR}/bl2-fake.bin

                # Init FIP DDR cert settings
                FIP_DDRCERTCONF="--stm32mp-cfg-cert  ${B}/${config}-${dt}/stm32mp_cfg_cert_ddr.crt"
                # Generate FIP DDR certificates
                if [ -f "${FIP_DEPLOYDIR_FWDDR}/${FIP_FW_DDR}-${dt}.${FIP_FW_DDR_SUFFIX}" ]; then
                    bbnote "${CERTTOOL} -n --tfw-nvctr 0  \
                            --rot-key ${sign_key} \
                            --rot-key-pwd $sign_single_key_pass \
                            ${FIP_DDRCERTCONF} \
                            ${CERT_DDRCONF}"
                    ${CERTTOOL} -n --tfw-nvctr 0 \
                            --rot-key ${sign_key} \
                            --rot-key-pwd $sign_single_key_pass \
                            ${FIP_DDRCERTCONF} \
                            ${CERT_DDRCONF}
                fi
            else
                FIP_CERTCONF=""
                FIP_DDRCERTCONF=""
            fi
            # Generate FIP binary
            bbnote "${FIPTOOL} create \
                            ${FIP_FWCONFIG} \
                            ${FIP_HWCONFIG} \
                            ${FIP_NTFW} \
                            ${FIP_BL31CONF} \
                            ${FIP_EXTRACONF} \
                            ${FIP_DDRCONF} \
                            ${FIP_CERTCONF} \
                            ${FIP_DEPLOYDIR_FIP}/${FIP_BASENAME}-${dt}-${config}${FIP_ENCRYPT_SUFFIX}${FIP_SIGN_SUFFIX}.${FIP_SUFFIX}"
            ${FIPTOOL} create \
                            ${FIP_FWCONFIG} \
                            ${FIP_HWCONFIG} \
                            ${FIP_NTFW} \
                            ${FIP_BL31CONF} \
                            ${FIP_EXTRACONF} \
                            ${FIP_DDRCONF} \
                            ${FIP_CERTCONF} \
                            ${FIP_DEPLOYDIR_FIP}/${FIP_BASENAME}-${dt}-${config}${FIP_ENCRYPT_SUFFIX}${FIP_SIGN_SUFFIX}.${FIP_SUFFIX}
            # Generate FIP DDR binary
            if [ -f "${FIP_DEPLOYDIR_FWDDR}/${FIP_FW_DDR}-${dt}.${FIP_FW_DDR_SUFFIX}" ]; then
                bbnote "${FIPTOOL} create \
                        ${FIP_DDRCERTCONF} \
                        ${FIP_DDRCONF} \
                        ${FIP_DEPLOYDIR_FIP}/${FIP_BASENAME}-${dt}-ddr${FIP_ENCRYPT_SUFFIX}${FIP_SIGN_SUFFIX}.${FIP_SUFFIX}"
                ${FIPTOOL} create \
                        ${FIP_DDRCERTCONF} \
                        ${FIP_DDRCONF} \
                        ${FIP_DEPLOYDIR_FIP}/${FIP_BASENAME}-${dt}-ddr${FIP_ENCRYPT_SUFFIX}${FIP_SIGN_SUFFIX}.${FIP_SUFFIX}
            fi
        done
    done
}

# Stub do_compile for nativesdk use case as we only expect to provide FIPTOOL_WRAPPER
do_compile:class-nativesdk() {
    return
}

do_install:class-nativesdk() {
    # Create the FIPTOOL_WRAPPER script to use on sdk side
    cat << EOF > ${WORKDIR}/${FIPTOOL_WRAPPER}
#!/bin/bash -
function bbfatal() { echo "\$*" ; exit 1 ; }

# Set default TF-A FIP config
FIP_CONFIG="\${FIP_CONFIG:-${FIP_CONFIG}}"
FIP_BL31_ENABLE="\${FIP_BL31_ENABLE:-${FIP_BL31_ENABLE}}"
FIP_BL32_CONF=""
FIP_DEVICETREE="\${FIP_DEVICETREE:-}"

# Set default supported configuration for devicetree and bl32 configuration
declare -A FIP_BL32_CONF_ARRAY
declare -A FIP_DEVICETREE_ARRAY
EOF
    for config in ${FIP_CONFIG}; do
        i=$(expr $i + 1)
        cat << EOF >> ${WORKDIR}/${FIPTOOL_WRAPPER}
FIP_BL32_CONF_ARRAY[${config}]="$(echo ${FIP_BL32_CONF} | cut -d',' -f${i})"
FIP_DEVICETREE_ARRAY[${config}]="$(echo ${FIP_DEVICETREE} | cut -d',' -f${i})"
EOF
    done
    unset i
    cat << EOF >> ${WORKDIR}/${FIPTOOL_WRAPPER}

# Make sure about FIP_CONFIG value
if [ -z "\$FIP_CONFIG" ]; then
    bbfatal "Wrong configuration 'FIP_CONFIG' is empty."
else
    # Check that configuration match any of the supported ones
    for config in \$FIP_CONFIG; do
        CONFIG_FOUND=NO
        for fip_config in ${FIP_CONFIG}; do
            [ "\${config}" = "\${fip_config}" ] && { CONFIG_FOUND="YES" ; break; }
        done
        [ "\${CONFIG_FOUND}" = "NO" ] && bbfatal "Wrong 'FIP_CONFIG' configuration : \${config} is not one of the supported one (${FIP_CONFIG})"
    done
fi
# Manage FIP_BL32_CONF default init
if [ -z "\$FIP_BL32_CONF" ]; then
    # Assigned default supported value
    for config in \$FIP_CONFIG; do
        FIP_BL32_CONF+="\${FIP_BL32_CONF_ARRAY[\${config}]},"
    done
fi
# Manage FIP_DEVICETREE default init
if [ -z "\$FIP_DEVICETREE" ]; then
    # Assigned default supported value
    for config in \$FIP_CONFIG; do
        FIP_DEVICETREE+="\${FIP_DEVICETREE_ARRAY[\${config}]},"
    done
fi

# Configure default folder path for binaries to package
FIP_DEPLOYDIR_ROOT="\${FIP_DEPLOYDIR_ROOT:-}"
FIP_DEPLOYDIR_FIP="\${FIP_DEPLOYDIR_FIP:-\$FIP_DEPLOYDIR_ROOT/fip}"
FIP_DEPLOYDIR_TFA="\${FIP_DEPLOYDIR_TFA:-\$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware/bl32}"
FIP_DEPLOYDIR_BL31="\${FIP_DEPLOYDIR_BL31:-\$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware/bl31}"
FIP_DEPLOYDIR_FWDDR="\${FIP_DEPLOYDIR_FWDDR:-\$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware/ddr}"
FIP_DEPLOYDIR_FWCONF="\${FIP_DEPLOYDIR_FWCONF:-\$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware/fwconfig}"
FIP_DEPLOYDIR_OPTEE="\${FIP_DEPLOYDIR_OPTEE:-\$FIP_DEPLOYDIR_ROOT/optee}"
FIP_DEPLOYDIR_UBOOT="\${FIP_DEPLOYDIR_UBOOT:-\$FIP_DEPLOYDIR_ROOT/u-boot}"

echo ""
echo "${FIPTOOL_WRAPPER} config:"
for config in \$FIP_CONFIG; do
    i=\$(expr \$i + 1)
    bl32_conf=\$(echo \$FIP_BL32_CONF | cut -d',' -f\$i)
    dt_config=\$(echo \$FIP_DEVICETREE | cut -d',' -f\$i)
    echo "  \${config}:" ; \\
    echo "    bl32 config value: \${bl32_conf}"
    echo "    devicetree config: \${dt_config}"
done
echo ""
echo "Switch configuration:"
echo "  FIP_BL31_ENABLE : \$FIP_BL31_ENABLE"
echo ""
echo "Output folders:"
echo "  FIP_DEPLOYDIR_ROOT  : \$FIP_DEPLOYDIR_ROOT"
echo "  FIP_DEPLOYDIR_FIP   : \$FIP_DEPLOYDIR_FIP"
echo "  FIP_DEPLOYDIR_TFA   : \$FIP_DEPLOYDIR_TFA"
echo "  FIP_DEPLOYDIR_BL31  : \$FIP_DEPLOYDIR_BL31"
echo "  FIP_DEPLOYDIR_FWCONF: \$FIP_DEPLOYDIR_FWCONF"
echo "  FIP_DEPLOYDIR_OPTEE : \$FIP_DEPLOYDIR_OPTEE"
echo "  FIP_DEPLOYDIR_UBOOT : \$FIP_DEPLOYDIR_UBOOT"
echo ""
unset i
for config in \$FIP_CONFIG; do
    i=\$(expr \$i + 1)
    bl32_conf=\$(echo \$FIP_BL32_CONF | cut -d',' -f\$i)
    dt_config=\$(echo \$FIP_DEVICETREE | cut -d',' -f\$i)
    for dt in \${dt_config}; do
        # Init soc suffix
        soc_suffix=""
        if [ -n "${STM32MP_SOC_NAME}" ]; then
            for soc in ${STM32MP_SOC_NAME}; do
                [ "\$(echo \${dt} | grep -c \${soc})" -eq 1 ] && soc_suffix="-\${soc}"
            done
        fi
        # Init FIP fw-config settings
        [ -f "\$FIP_DEPLOYDIR_FWCONF/\${dt}-${FIP_FW_CONFIG}-\${config}.${FIP_FW_CONFIG_SUFFIX}" ] || bbfatal "Missing \${dt}-${FIP_FW_CONFIG}-\${config}.${FIP_FW_CONFIG_SUFFIX} file in folder: \\\$FIP_DEPLOYDIR_FWCONF or '\\\$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware/fwconfig'"
        FIP_FWCONFIG="--fw-config \$FIP_DEPLOYDIR_FWCONF/\${dt}-${FIP_FW_CONFIG}-\${config}.${FIP_FW_CONFIG_SUFFIX}"
        # Init FIP hw-config settings
        [ -f "\$FIP_DEPLOYDIR_UBOOT/${FIP_UBOOT_DTB}-\${dt}.${FIP_UBOOT_DTB_SUFFIX}" ] || bbfatal "Missing ${FIP_UBOOT_DTB}-\${dt}.${FIP_UBOOT_DTB_SUFFIX} file in folder: '\\\$FIP_DEPLOYDIR_UBOOT' or '\\\$FIP_DEPLOYDIR_ROOT/u-boot'"
        FIP_HWCONFIG="--hw-config \$FIP_DEPLOYDIR_UBOOT/${FIP_UBOOT_DTB}-\${dt}.${FIP_UBOOT_DTB_SUFFIX}"
        # Init FIP nt-fw config
        [ -f "\$FIP_DEPLOYDIR_UBOOT/${FIP_UBOOT}\${soc_suffix}.${FIP_UBOOT_SUFFIX}" ] || bbfatal "Missing ${FIP_UBOOT}\${soc_suffix}.${FIP_UBOOT_SUFFIX} file in folder: '\\\$FIP_DEPLOYDIR_UBOOT' or '\\\$FIP_DEPLOYDIR_ROOT/u-boot'"
        FIP_NTFW="--nt-fw \$FIP_DEPLOYDIR_UBOOT/${FIP_UBOOT}\${soc_suffix}.${FIP_UBOOT_SUFFIX}"
        # Init FIP bl31 settings
        if [ "\$FIP_BL31_ENABLE" = "1" ]; then
            # Check for files
            [ -f "\$FIP_DEPLOYDIR_BL31/${FIP_BL31}\${soc_suffix}.${FIP_BL31_SUFFIX}" ] || bbfatal "Missing \$FIP_DEPLOYDIR_BL31/${FIP_BL31}\${soc_suffix}.${FIP_BL31_SUFFIX} file in folder: '\\\$FIP_DEPLOYDIR_BL31' or '\\\$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware/bl31'"
            [ -f "\$FIP_DEPLOYDIR_BL31/\${dt}-${FIP_BL31_DTB}.${FIP_BL31_DTB_SUFFIX}" ] || bbfatal "Missing \${dt}-${FIP_BL31_DTB}.${FIP_BL31_DTB_SUFFIX} file in folder: '\\\$FIP_DEPLOYDIR_BL31' or '\\\$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware/bl31'"
            # Set FIP_BL31CONF
            FIP_BL31CONF="\\
                --soc-fw \$FIP_DEPLOYDIR_BL31/${FIP_BL31}\${soc_suffix}.${FIP_BL31_SUFFIX} \\
                --soc-fw-config \$FIP_DEPLOYDIR_BL31/\${dt}-${FIP_BL31_DTB}.${FIP_BL31_DTB_SUFFIX} \\
                "
        else
            FIP_BL31CONF=""
        fi
        # Init FIP extra conf settings
        if [ "\${bl32_conf}" = "${FIP_CONFIG_FW_TFA}" ]; then
            # Check for files
            [ -f "\$FIP_DEPLOYDIR_TFA/${FIP_TFA}\${soc_suffix}.${FIP_TFA_SUFFIX}" ] || bbfatal "Missing ${FIP_TFA}\${soc_suffix}.${FIP_TFA_SUFFIX} file in folder: '\\\$FIP_DEPLOYDIR_TFA' or '\\\$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware/bl32'"
            [ -f "\$FIP_DEPLOYDIR_TFA/\${dt}-${FIP_TFA_DTB}.${FIP_TFA_DTB_SUFFIX}" ] || bbfatal "Missing \${dt}-${FIP_TFA_DTB}.${FIP_TFA_DTB_SUFFIX} file in folder: '\\\$FIP_DEPLOYDIR_TFA' or '\\\$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware/bl32'"
            # Set FIP_EXTRACONF
            FIP_EXTRACONF="\\
                --tos-fw \$FIP_DEPLOYDIR_TFA/${FIP_TFA}\${soc_suffix}.${FIP_TFA_SUFFIX} \\
                --tos-fw-config \$FIP_DEPLOYDIR_TFA/\${dt}-${FIP_TFA_DTB}.${FIP_TFA_DTB_SUFFIX} \\
                "
        elif [ "\${bl32_conf}" = "${FIP_CONFIG_FW_TEE}" ]; then
            # Check for files
            [ -f "\$FIP_DEPLOYDIR_OPTEE/${FIP_OPTEE_HEADER}-\${dt}.${FIP_OPTEE_SUFFIX}" ] || bbfatal "Missing ${FIP_OPTEE_HEADER}-\${dt}.${FIP_OPTEE_SUFFIX} file in folder: '\\\$FIP_DEPLOYDIR_OPTEE' or '\\\$FIP_DEPLOYDIR_ROOT/optee'"
            [ -f "\$FIP_DEPLOYDIR_OPTEE/${FIP_OPTEE_PAGER}-\${dt}.${FIP_OPTEE_SUFFIX}" ] || bbfatal "Missing ${FIP_OPTEE_PAGER}-\${dt}.${FIP_OPTEE_SUFFIX} file in folder: '\\\$FIP_DEPLOYDIR_OPTEE' or '\\\$FIP_DEPLOYDIR_ROOT/optee'"
            [ -f "\$FIP_DEPLOYDIR_OPTEE/${FIP_OPTEE_PAGEABLE}-\${dt}.${FIP_OPTEE_SUFFIX}" ] || bbfatal "Missing ${FIP_OPTEE_PAGEABLE}-\${dt}.${FIP_OPTEE_SUFFIX} file in folder: '\\\$FIP_DEPLOYDIR_OPTEE' or '\\\$FIP_DEPLOYDIR_ROOT/optee'"
            # Set FIP_EXTRACONF
            FIP_EXTRACONF="\\
                --tos-fw \$FIP_DEPLOYDIR_OPTEE/${FIP_OPTEE_HEADER}-\${dt}.${FIP_OPTEE_SUFFIX} \\
                --tos-fw-extra1 \$FIP_DEPLOYDIR_OPTEE/${FIP_OPTEE_PAGER}-\${dt}.${FIP_OPTEE_SUFFIX} \\
                --tos-fw-extra2 \$FIP_DEPLOYDIR_OPTEE/${FIP_OPTEE_PAGEABLE}-\${dt}.${FIP_OPTEE_SUFFIX} \\
                "
        else
            bbfatal "Wrong configuration '\${bl32_conf}' found in FIP_CONFIG for \${config} config."
        fi

        # DRR FW
        if [ -f "\$FIP_DEPLOYDIR_FWDDR/${FIP_FW_DDR}-\${dt}.${FIP_FW_DDR_SUFFIX}" ]; then
            FIP_EXTRACONF="\$FIP_EXTRACONF --ddr-fw \$FIP_DEPLOYDIR_FWDDR/${FIP_FW_DDR}-\${dt}.${FIP_FW_DDR_SUFFIX} "
            ${FIPTOOL} create \\
                    --ddr-fw \$FIP_DEPLOYDIR_FWDDR/${FIP_FW_DDR}-\${dt}.${FIP_FW_DDR_SUFFIX} \\
                    \$FIP_DEPLOYDIR_FIP/${FIP_BASENAME}-\${dt}-ddr.${FIP_SUFFIX}
            echo "[${FIPTOOL}] DDR FW created"
        fi

        # Generate FIP binary
        echo "[${FIPTOOL}] Create ${FIP_BASENAME}-\${dt}-\${config}.${FIP_SUFFIX} fip binary into 'FIP_DEPLOYDIR_FIP' folder..."
        [ -d "\$FIP_DEPLOYDIR_FIP" ] || mkdir -p "\$FIP_DEPLOYDIR_FIP"
        ${FIPTOOL} create \\
                        \$FIP_FWCONFIG \\
                        \$FIP_HWCONFIG \\
                        \$FIP_NTFW \\
                        \$FIP_BL31CONF \\
                        \$FIP_EXTRACONF \\
                        \$FIP_DEPLOYDIR_FIP/${FIP_BASENAME}-\${dt}-\${config}.${FIP_SUFFIX}
        echo "[${FIPTOOL}] Done"
    done
done
EOF

    # Install the FIPTOOL_WRAPPER
    install -d ${D}${bindir}
    install -m 0755 ${WORKDIR}/${FIPTOOL_WRAPPER} ${D}${bindir}/
}

# Feed package for sdk with our fiptool wrapper
FILES:${FIPTOOL_WRAPPER}:class-nativesdk = "${bindir}/${FIPTOOL_WRAPPER}"

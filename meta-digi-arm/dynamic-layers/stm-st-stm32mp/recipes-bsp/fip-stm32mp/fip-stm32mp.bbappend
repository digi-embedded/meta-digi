#
# Copyright (C) 2024,2025, Digi International Inc.
#

# Inherit custom DIGI sign class to skip signing tool and key parsing restrictions
inherit sign-stm32mp-digi

# Add optee-usb FIP configuration
STM32MP_DEVICETREE_USB = " ${@' '.join('%s' % dt_file for dt_file in list(dict.fromkeys((d.getVar('STM32MP_DT_FILES_USB') or '').split())))} "
FIP_CONFIG[optee-usb]  ?= "optee,${STM32MP_DEVICETREE_USB},default:optee,usb"
FIP_CONFIG += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', bb.utils.contains('BOOTDEVICE_LABELS', 'usb', 'optee-usb', '', d), '', d)}"

# Obtain password to use in FIP generation
# Get password from file using the given key index
do_deploy[prefuncs] += "${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', 'set_fip_sign_key', '', d)}"
python set_fip_sign_key() {
    passfile = d.getVar('TRUSTFENCE_PASSWORD_FILE')
    if (os.path.isfile(passfile)):
        with open(passfile, "r") as file:
            p = file.read().strip()
            if (p):
                d.setVar('SIGN_KEY_PASS', p)
}

# Addons parameters for FIP_WRAPPER
FIP_SOC_SEARCH ?= " ${STM32MP_SOC_NAME} "
FIP_SOC_MATCH ?= " ${DIGI_SOM} "

FWDDR_SUFFIX ?= "bin"

# Deploy the fip binary for current target
do_deploy() {
    install -d ${DEPLOYDIR}/${FIP_DIR_FIP}

    unset i
    for config in ${FIP_CONFIG}; do
        i=$(expr $i + 1)
        bl32_conf=$(echo ${FIP_BL32_CONF} | cut -d',' -f${i})
        dt_config=$(echo ${FIP_DEVICETREE} | cut -d',' -f${i})
        search_conf=$(echo ${FIP_SEARCH_CONF} | cut -d',' -f${i})
        device_conf=$(echo ${FIP_DEVICE_CONF} | cut -d',' -f${i})
        for dt in ${dt_config}; do
            # Init soc suffix
            soc_suffix="${FIP_SOC_SEARCH}"
            if [ -n "${STM32MP_SOC_NAME}" ]; then
                for soc in ${STM32MP_SOC_NAME}; do
                    [ "$(echo ${dt} | grep -c ${soc})" -eq 1 ] && soc_suffix="${soc}"
                done
            fi
            encrypt_key=""
            if [ "${ENCRYPT_ENABLE}" = "1" ]; then
                encrypt_key="${ENCRYPT_FIP_KEY_PATH_LIST}"
                if [ -n "${STM32MP_ENCRYPT_SOC_NAME}" ]; then
                    unset k
                    for soc in ${STM32MP_ENCRYPT_SOC_NAME}; do
                        k=$(expr $k + 1)
                        [ "$(echo ${dt} | grep -c ${soc})" -eq 1 ] && encrypt_key=$(echo ${ENCRYPT_FIP_KEY_PATH_LIST} | cut -d',' -f${k})
                    done
                fi
            fi
            # Init FIP bl31 settings
            FIP_PARAM_BLxx=""
            # Init FIP extra conf settings
            if [ "${bl32_conf}" = "${FIP_CONFIG_FW_TFA}" ]; then
                FIP_PARAM_BLxx="--use-bl32"
            elif [ "${bl32_conf}" = "${FIP_CONFIG_FW_TEE}" ]; then
                if [ "${FIP_BL31_ENABLE}" = "1" ]; then
                    FIP_PARAM_BLxx="--use-bl31"
                    if [ "${ENCRYPT_ENABLE}" = "1" ]; then
                         FIP_PARAM_BLxx="--use-bl31 --encrypt $encrypt_key"
                    fi
                else
                    if [ "${ENCRYPT_ENABLE}" = "1" ]; then
                        FIP_PARAM_BLxx="--encrypt $encrypt_key"
                    fi
                fi
            else
                bbfatal "Wrong configuration '${bl32_conf}' found in FIP_CONFIG for ${config} config."
            fi
            FIP_PARAM_SIGN=""
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
                        if [ "$(echo ${dt} | grep -c ${soc})" -eq 1 ] || [ "$(echo ${dt} | grep -c ${FIP_SOC_MATCH})" -eq 1 ] ;then
                            sign_key=$(echo ${SIGN_KEY_PATH_LIST} | cut -d',' -f${k})
                        fi
                    done
                fi
                FIP_PARAM_SIGN="--sign --signature-key $sign_key --signature-key-pass $sign_single_key_pass"
            fi

            # Configure storage search
            STORAGE_SEARCH=""
            [ -z "${device_conf}" ] || STORAGE_SEARCH="--search-storage ${device_conf}"

            FIP_PARAM_ddr=""
            if [ -d "${RECIPE_SYSROOT}/${FIP_DIR_TFA_BASE}/${FIP_DIR_FWDDR}" ]; then
                FIP_PARAM_ddr="--use-ddr"
                echo "********************************************"
                bbnote "[fip-utils-stm32mp] FIP DDR command details:\
                FIP_DEPLOYDIR_ROOT=${RECIPE_SYSROOT} \
                ${FIP_WRAPPER} \
                    ${FIP_PARAM_BLxx} \
                    ${FIP_PARAM_SIGN} \
                    ${STORAGE_SEARCH} \
                    --use-ddr --generate-only-ddr \
                    --search-configuration ${config}\
                    --search-devicetree ${dt} \
                    --search-soc-name ${soc_suffix} \
                    --output ${DEPLOYDIR}/${FIP_DIR_FIP}"
                echo "********************************************"
                FIP_DEPLOYDIR_ROOT="${RECIPE_SYSROOT}" \
                ${FIP_WRAPPER} \
                    ${FIP_PARAM_BLxx} \
                    ${FIP_PARAM_SIGN} \
                    ${STORAGE_SEARCH} \
                    --use-ddr --generate-only-ddr \
                    --search-configuration ${config}\
                    --search-devicetree ${dt} \
                    --search-soc-name ${soc_suffix} \
                    --output ${DEPLOYDIR}/${FIP_DIR_FIP}
            fi
            # Configure secondary config search
            SECOND_CONFSEARCH=""
            [ -z "${search_conf}" ] || SECOND_CONFSEARCH="--search-secondary-config ${search_conf}"
            echo "****************************************"
            bbnote "[fip-utils-stm32mp] FIP command details:\
            \nFIP_DEPLOYDIR_ROOT=${RECIPE_SYSROOT} \
            \n${FIP_WRAPPER} \
                    \n${FIP_PARAM_BLxx} \
                    \n${FIP_PARAM_SIGN} \
                    \n${FIP_PARAM_ddr} \
                    \n${STORAGE_SEARCH} \
                    \n${SECOND_CONFSEARCH} \
                    \n--search-configuration ${config} \
                    \n--search-devicetree ${dt} \
                    \n--search-soc-name ${soc_suffix} \
                    \n--output ${DEPLOYDIR}/${FIP_DIR_FIP}"
            echo "****************************************"
            FIP_DEPLOYDIR_ROOT="${RECIPE_SYSROOT}" \
            ${FIP_WRAPPER} \
                    ${FIP_PARAM_BLxx} \
                    ${FIP_PARAM_SIGN} \
                    ${FIP_PARAM_ddr} \
                    ${STORAGE_SEARCH} \
                    ${SECOND_CONFSEARCH} \
                    --search-configuration ${config} \
                    --search-devicetree ${dt} \
                    --search-soc-name ${soc_suffix} \
                    --output ${DEPLOYDIR}/${FIP_DIR_FIP}
        done
    done

    # Create symlinks in DEPLOYDIR

    # Remove trailing slash (/) from ST variables
    FIP_BASEDIR="$(echo ${FIP_DIR_FIP} | cut -c2-)"
    unset i
    for config in ${FIP_CONFIG}; do
        i="$(expr ${i} + 1)"
        dt_config=$(echo ${FIP_DEVICETREE} | cut -d',' -f${i})
        for dt in ${dt_config}; do
            FIP_FILENAME="${FIP_BASENAME}-${dt}-${config}${FIP_ENCRYPT_SUFFIX}${FIP_SIGN_SUFFIX}.${FIP_SUFFIX}"
            if [ -f "${DEPLOYDIR}/${FIP_BASEDIR}/${FIP_FILENAME}" ]; then
                cd "${DEPLOYDIR}"
                # symlink FIP
                ln -sf "${FIP_BASEDIR}/${FIP_FILENAME}" "${DEPLOYDIR}/"
            fi

            FIP_DDR_FILENAME="${FIP_BASENAME}-${dt}-ddr-${config}${FIP_ENCRYPT_SUFFIX}${FIP_SIGN_SUFFIX}.${FWDDR_SUFFIX}"
            if [ -f "${DEPLOYDIR}/${FIP_BASEDIR}/${FIP_DDR_FILENAME}" ]; then
                cd "${DEPLOYDIR}"
                # symlink DDR firmware (needed for USB recovery)
                ln -sf "${FIP_BASEDIR}/${FIP_DDR_FILENAME}" "${DEPLOYDIR}/"
            fi
        done
    done
}
addtask deploy before do_build after do_compile
